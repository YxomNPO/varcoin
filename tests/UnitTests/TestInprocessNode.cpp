// Copyright (c) 2012-2018, The VarNote developers, YxomTech
//
// This file is part of Varcoin.
//
// Varcoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Varcoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Varcoin.  If not, see <http://www.gnu.org/licenses/>.

#include "gtest/gtest.h"

#include <system_error>

#include <boost/range/combine.hpp>

#include "EventWaiter.h"
#include "ICoreStub.h"
#include "IVarNoteProtocolQueryStub.h"
#include "InProcessNode/InProcessNode.h"
#include "TestBlockchainGenerator.h"
#include "Logging/FileLogger.h"
#include "VarNoteCore/TransactionApi.h"
#include "VarNoteCore/VarNoteTools.h"
#include "VarNoteCore/VerificationContext.h"
#include "Common/StringTools.h"
#include "System/Context.h"
#include "System/Timer.h"

#include <System/RemoteContext.h>
#include <System/RemoteEventLock.h>

using namespace Crypto;
using namespace VarNote;
using namespace Common;

namespace {
struct CallbackStatus {
  explicit CallbackStatus(System::Dispatcher& dispatcher): dispatcher(dispatcher), waiter(dispatcher) {}

  bool wait(std::chrono::milliseconds timeout = std::chrono::milliseconds(3000)) {
    System::Context<> waitingContext(dispatcher, [&] {
      waiter.wait();
    });

    System::Context<> timeoutContext(dispatcher, [&] {
      try {
        System::Timer(dispatcher).sleep(timeout);
        waitingContext.interrupt();
      } catch (System::InterruptedException&) {
      }
    });

    try {
      waitingContext.get();
      return true;
    } catch (System::InterruptedException&) {
      return false;
    }
  }

  bool ok(std::chrono::milliseconds timeout = std::chrono::milliseconds(3000)) {
    return wait(timeout) && !static_cast<bool>(code);
  }

  void setStatus(const std::error_code& ec) { code = ec; waiter.set(); }

  void setStatusRemote(const std::error_code& ec) { 
    System::Event e(dispatcher);
    {
      dispatcher.remoteSpawn([this, &ec, &e]() {
        code = ec;
        waiter.set();
        e.set();
      });
      System::RemoteEventLock rel(dispatcher, e);
    }
  }

  std::error_code getStatus() const { return code; }

  std::error_code code;
  System::Dispatcher& dispatcher;
  System::Event waiter;
};

VarNote::Transaction createTx(VarNote::ITransactionReader& tx) {
  VarNote::Transaction outTx;
  fromBinaryArray(outTx, tx.getTransactionData());
  return outTx;
}

class InProcessNodeTests : public ::testing::Test {
public:
  InProcessNodeTests() :
    node(coreStub, protocolQueryStub, dispatcher),
    currency(VarNote::CurrencyBuilder(logger).currency()),
    generator(currency) {}
  void SetUp() override;

protected:
  void initNode();

  template<class ResultType>
  ResultType invokeNodeMethod(std::function<ResultType()>&& function);

  System::Dispatcher dispatcher;
  ICoreStub coreStub;
  IVarNoteProtocolQueryStub protocolQueryStub;
  VarNote::InProcessNode node;

  VarNote::Currency currency;
  TestBlockchainGenerator generator;
  Logging::FileLogger logger;
};

void InProcessNodeTests::SetUp() {
  logger.init("/dev/null");
  for (auto iter = generator.getBlockchain().begin(); iter != generator.getBlockchain().end(); iter++) {
    coreStub.addBlock(*iter);
  }

  initNode();
}
}

void InProcessNodeTests::initNode() {
  CallbackStatus status(dispatcher);

  node.init([&status] (std::error_code ec) { status.setStatusRemote(ec); });
  ASSERT_TRUE(status.ok());
}

template<class ResultType>
ResultType InProcessNodeTests::invokeNodeMethod(std::function<ResultType()>&& function) {
  System::RemoteContext<ResultType> context(dispatcher, std::move(function));
  return context.get();
}

TEST_F(InProcessNodeTests, initOk) {
  VarNote::InProcessNode newNode(coreStub, protocolQueryStub, dispatcher);
  CallbackStatus status(dispatcher);

  newNode.init([&status] (std::error_code ec) { status.setStatusRemote(ec); });
  ASSERT_TRUE(status.ok());
}

TEST_F(InProcessNodeTests, doubleInit) {
  CallbackStatus status(dispatcher);
  node.init([&status] (std::error_code ec) { status.setStatusRemote(ec); });
  ASSERT_TRUE(status.wait());

  std::error_code ec = status.getStatus();
  ASSERT_NE(ec, std::error_code());
}

TEST_F(InProcessNodeTests, shutdownNotInited) {
  VarNote::InProcessNode newNode(coreStub, protocolQueryStub, dispatcher);
  ASSERT_FALSE(newNode.shutdown());
}

TEST_F(InProcessNodeTests, shutdown) {
  ASSERT_TRUE(node.shutdown());
}

TEST_F(InProcessNodeTests, getPeersCountSuccess) {
  protocolQueryStub.setPeerCount(1);
  size_t peerCount = invokeNodeMethod<size_t>([this] () {return node.getPeerCount(); });
  ASSERT_EQ(1, peerCount);
}

TEST_F(InProcessNodeTests, getLastLocalBlockHeightSuccess) {
  const size_t NUMBER_OF_BLOCKS = 5;
  uint32_t index = coreStub.getTopBlockIndex();
  generator.generateEmptyBlocks(NUMBER_OF_BLOCKS);
  ASSERT_GE(generator.getBlockchain().size(), NUMBER_OF_BLOCKS);
  for (auto iter = generator.getBlockchain().begin() + index + 1; iter != generator.getBlockchain().end(); iter++) {
    coreStub.addBlock(*iter);
  }

  System::Timer timer(dispatcher);
  timer.sleep(std::chrono::milliseconds(100)); //bad inproc node architecture

  auto blockHeight = invokeNodeMethod<uint32_t>([this] () { return node.getLastLocalBlockHeight(); });
  ASSERT_EQ(index + NUMBER_OF_BLOCKS, blockHeight);
}

TEST_F(InProcessNodeTests, getLastKnownBlockHeightSuccess) {
  protocolQueryStub.setObservedHeight(10);
  auto lastKnownBlockHeight = invokeNodeMethod<uint32_t>([this] () { return node.getLastKnownBlockHeight() + 1; });
  ASSERT_EQ(10, lastKnownBlockHeight);
}

TEST_F(InProcessNodeTests, getTransactionOutsGlobalIndicesSuccess) {
  Crypto::Hash ignore;
  std::vector<uint32_t> indices;
  std::vector<uint32_t> expectedIndices;

  uint64_t start = 10;
  std::generate_n(std::back_inserter(expectedIndices), 5, [&start] () { return start++; });
  coreStub.set_outputs_gindexs(expectedIndices, true);

  CallbackStatus status(dispatcher);
  node.getTransactionOutsGlobalIndices(ignore, indices, [&status] (std::error_code ec) { status.setStatusRemote(ec); });
  ASSERT_TRUE(status.ok());

  ASSERT_EQ(expectedIndices.size(), indices.size());
  std::sort(indices.begin(), indices.end());
  ASSERT_TRUE(std::equal(indices.begin(), indices.end(), expectedIndices.begin()));
}

TEST_F(InProcessNodeTests, getTransactionOutsGlobalIndicesFailure) {
  Crypto::Hash ignore;
  std::vector<uint32_t> indices;
  coreStub.set_outputs_gindexs(indices, false);

  CallbackStatus status(dispatcher);
  node.getTransactionOutsGlobalIndices(ignore, indices, [&status] (std::error_code ec) { status.setStatusRemote(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_NE(std::error_code(), status.getStatus());
}

TEST_F(InProcessNodeTests, getRandomOutsByAmountsSuccess) {
  Crypto::PublicKey ignoredPublicKey;
  Crypto::SecretKey ignoredSectetKey;
  Crypto::generate_keys(ignoredPublicKey, ignoredSectetKey);

  VarNote::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_response expectedResp;
  VarNote::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_outs_for_amount out;
  out.amount = 10;
  out.outs.push_back({ 11, ignoredPublicKey });
  expectedResp.outs.push_back(out);
  coreStub.set_random_outs(expectedResp, true);

  std::vector<VarNote::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_outs_for_amount> outs;

  CallbackStatus status(dispatcher);
  node.getRandomOutsByAmounts({10}, 1, outs, [&status] (std::error_code ec) { status.setStatusRemote(ec); });
  ASSERT_TRUE(status.ok());
  ASSERT_EQ(1, outs.size());

  ASSERT_EQ(10, outs[0].amount);
  ASSERT_EQ(1, outs[0].outs.size());
  ASSERT_EQ(11, outs[0].outs.front().global_amount_index);
}

TEST_F(InProcessNodeTests, getRandomOutsByAmountsFailure) {
  VarNote::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_response expectedResp;
  coreStub.set_random_outs(expectedResp, false);

  std::vector<VarNote::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_outs_for_amount> outs;

  CallbackStatus status(dispatcher);
  node.getRandomOutsByAmounts({1,2,3}, 1, outs, [&status] (std::error_code ec) { status.setStatusRemote(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_NE(std::error_code(), status.getStatus());
}

TEST_F(InProcessNodeTests, getPeerCountUninitialized) {
  VarNote::InProcessNode newNode(coreStub, protocolQueryStub, dispatcher);
  ASSERT_ANY_THROW(newNode.getPeerCount());
}

TEST_F(InProcessNodeTests, getLastLocalBlockHeightUninitialized) {
  VarNote::InProcessNode newNode(coreStub, protocolQueryStub, dispatcher);
  ASSERT_ANY_THROW(newNode.getLastLocalBlockHeight());
}

TEST_F(InProcessNodeTests, getLastKnownBlockHeightUninitialized) {
  VarNote::InProcessNode newNode(coreStub, protocolQueryStub, dispatcher);
  ASSERT_ANY_THROW(newNode.getLastKnownBlockHeight());
}

TEST_F(InProcessNodeTests, getNewBlocksUninitialized) {
  VarNote::InProcessNode newNode(coreStub, protocolQueryStub, dispatcher);
  std::vector<Crypto::Hash> knownBlockIds;
  std::vector<VarNote::RawBlock> newBlocks;
  uint32_t startHeight = 0;

  CallbackStatus status(dispatcher);
  newNode.getNewBlocks(std::move(knownBlockIds), newBlocks, startHeight, [&] (std::error_code ec) { status.setStatus(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_NE(std::error_code(), status.getStatus());
}

TEST_F(InProcessNodeTests, getTransactionOutsGlobalIndicesUninitialized) {
  VarNote::InProcessNode newNode(coreStub, protocolQueryStub, dispatcher);
  std::vector<uint32_t> outsGlobalIndices;

  CallbackStatus status(dispatcher);
  newNode.getTransactionOutsGlobalIndices(Crypto::Hash(), outsGlobalIndices, [&](std::error_code ec) { status.setStatus(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_NE(std::error_code(), status.getStatus());
}

TEST_F(InProcessNodeTests, getRandomOutsByAmountsUninitialized) {
  VarNote::InProcessNode newNode(coreStub, protocolQueryStub, dispatcher);
  std::vector<VarNote::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_outs_for_amount> outs;

  CallbackStatus status(dispatcher);
  newNode.getRandomOutsByAmounts({ 1, 2, 3 }, 1, outs, [&](std::error_code ec) { status.setStatus(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_NE(std::error_code(), status.getStatus());
}

TEST_F(InProcessNodeTests, relayTransactionUninitialized) {
  VarNote::InProcessNode newNode(coreStub, protocolQueryStub, dispatcher);

  CallbackStatus status(dispatcher);
  newNode.relayTransaction(VarNote::Transaction(), [&](std::error_code ec) { status.setStatus(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_NE(std::error_code(), status.getStatus());
}

TEST_F(InProcessNodeTests, getBlocksByHeightEmpty) {
  std::vector<uint32_t> blockHeights;
  std::vector<std::vector<VarNote::BlockDetails>> blocks;
  ASSERT_EQ(blockHeights.size(), 0);
  ASSERT_EQ(blocks.size(), 0);

  CallbackStatus status(dispatcher);
  node.getBlocks(blockHeights, blocks, [&status] (std::error_code ec) { status.setStatusRemote(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_EQ(std::error_code(), status.getStatus());
}

TEST_F(InProcessNodeTests, getBlocksByHeightMany) {
  const size_t NUMBER_OF_BLOCKS = 10;

  std::vector<uint32_t> blockHeights;
  std::vector<std::vector<VarNote::BlockDetails>> actualBlocks;

  std::vector<VarNote::BlockTemplate> expectedBlocks;

  generator.generateEmptyBlocks(NUMBER_OF_BLOCKS);

  for (auto iter = generator.getBlockchain().begin() + 1; iter != generator.getBlockchain().end(); iter++) {
    expectedBlocks.push_back(*iter);
    blockHeights.push_back(std::move(boost::get<VarNote::BaseInput>(iter->baseTransaction.inputs.front()).blockIndex));
    coreStub.addBlock(*iter);
  }

  CallbackStatus status(dispatcher);
  node.getBlocks(blockHeights, actualBlocks, [&status] (std::error_code ec) { status.setStatusRemote(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_EQ(std::error_code(), status.getStatus());

  ASSERT_EQ(blockHeights.size(), expectedBlocks.size());
  ASSERT_EQ(blockHeights.size(), actualBlocks.size());
  auto range1 = boost::combine(blockHeights, expectedBlocks);
  auto range = boost::combine(range1, actualBlocks);
  for (const boost::tuple<boost::tuple<size_t, VarNote::BlockTemplate>, std::vector<VarNote::BlockDetails>>& sameHeight : range) {
    EXPECT_EQ(sameHeight.get<1>().size(), 1);
    for (const VarNote::BlockDetails& block : sameHeight.get<1>()) {
      EXPECT_EQ(block.index, sameHeight.get<0>().get<0>());
      auto expectedCryptoHash = VarNote::CachedBlock(sameHeight.get<0>().get<1>()).getBlockHash();
      Hash expectedHash = reinterpret_cast<const Hash&>(expectedCryptoHash);
      EXPECT_EQ(block.hash, expectedHash);
      EXPECT_FALSE(block.isAlternative);
    }
  }
}

TEST_F(InProcessNodeTests, getBlocksByHeightFail) {
  const size_t NUMBER_OF_BLOCKS = 10;

  std::vector<uint32_t> blockHeights;
  std::vector<std::vector<VarNote::BlockDetails>> actualBlocks;

  generator.generateEmptyBlocks(NUMBER_OF_BLOCKS);

  for (const VarNote::BlockTemplate& block : generator.getBlockchain()) {
    coreStub.addBlock(block);
  }

  for (uint32_t i = 0; i < NUMBER_OF_BLOCKS * 2; ++i) {
    blockHeights.push_back(i);
  }

  CallbackStatus status(dispatcher);
  node.getBlocks(blockHeights, actualBlocks, [&status] (std::error_code ec) { status.setStatusRemote(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_NE(std::error_code(), status.getStatus());
}

TEST_F(InProcessNodeTests, getBlocksByHeightNotInited) {
  VarNote::InProcessNode newNode(coreStub, protocolQueryStub, dispatcher);

  std::vector<uint32_t> blockHeights;
  std::vector<std::vector<VarNote::BlockDetails>> blocks;
  ASSERT_EQ(blockHeights.size(), 0);
  ASSERT_EQ(blocks.size(), 0);

  CallbackStatus status(dispatcher);
  newNode.getBlocks(blockHeights, blocks, [&status] (std::error_code ec) { status.setStatus(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_NE(std::error_code(), status.getStatus());
}

TEST_F(InProcessNodeTests, getBlocksByHashEmpty) {
  std::vector<Crypto::Hash> blockHashes;
  std::vector<VarNote::BlockDetails> blocks;
  ASSERT_EQ(blockHashes.size(), 0);
  ASSERT_EQ(blocks.size(), 0);

  CallbackStatus status(dispatcher);
  node.getBlocks(blockHashes, blocks, [&status] (std::error_code ec) { status.setStatusRemote(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_EQ(std::error_code(), status.getStatus());
}

TEST_F(InProcessNodeTests, getBlocksByHashMany) {
  const size_t NUMBER_OF_BLOCKS = 10;

  std::vector<Crypto::Hash> blockHashes;
  std::vector<VarNote::BlockDetails> actualBlocks;

  std::vector<VarNote::BlockTemplate> expectedBlocks;

  generator.generateEmptyBlocks(NUMBER_OF_BLOCKS);
  ASSERT_GE(generator.getBlockchain().size(), NUMBER_OF_BLOCKS);

  for (auto iter = generator.getBlockchain().begin() + 1; iter != generator.getBlockchain().end(); iter++) {
    expectedBlocks.push_back(*iter);
    blockHashes.push_back(VarNote::CachedBlock(*iter).getBlockHash());
    coreStub.addBlock(*iter);
  }

  ASSERT_GE(blockHashes.size(), NUMBER_OF_BLOCKS);
  ASSERT_EQ(blockHashes.size(), expectedBlocks.size());
  ASSERT_EQ(actualBlocks.size(), 0);

  CallbackStatus status(dispatcher);
  node.getBlocks(blockHashes, actualBlocks, [&status] (std::error_code ec) { status.setStatusRemote(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_EQ(std::error_code(), status.getStatus());

  ASSERT_EQ(blockHashes.size(), expectedBlocks.size());
  ASSERT_EQ(blockHashes.size(), actualBlocks.size());
  auto range1 = boost::combine(blockHashes, expectedBlocks);
  auto range = boost::combine(range1, actualBlocks);
  for (const boost::tuple<boost::tuple<Crypto::Hash, VarNote::BlockTemplate>, VarNote::BlockDetails>& sameHeight : range) {
    Crypto::Hash expectedCryptoHash = VarNote::CachedBlock(sameHeight.get<0>().get<1>()).getBlockHash();
    EXPECT_EQ(expectedCryptoHash, sameHeight.get<0>().get<0>());
    Hash expectedHash = reinterpret_cast<const Hash&>(expectedCryptoHash);
    EXPECT_EQ(sameHeight.get<1>().hash, expectedHash);
    EXPECT_FALSE(sameHeight.get<1>().isAlternative);
  }
}

/*
TEST_F(InProcessNodeTests, getBlocksByHashFail) {
  const size_t NUMBER_OF_BLOCKS = 10;

  std::vector<Crypto::Hash> blockHashes;
  std::vector<VarNote::BlockDetails> actualBlocks;

  generator.generateEmptyBlocks(NUMBER_OF_BLOCKS);
  ASSERT_LT(generator.getBlockchain().size(), NUMBER_OF_BLOCKS * 2);

  for (const VarNote::BlockTemplate& block : generator.getBlockchain()) {
    coreStub.addBlock(block);
  }

  for (uint32_t i = 0; i < NUMBER_OF_BLOCKS * 2; ++i) {
    blockHashes.push_back(boost::value_initialized<Crypto::Hash>());
  }

  ASSERT_EQ(actualBlocks.size(), 0);

  CallbackStatus status(dispatcher);
  node.getBlocks(blockHashes, actualBlocks, [&status] (std::error_code ec) { status.setStatusRemote(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_NE(std::error_code(), status.getStatus());
}

TEST_F(InProcessNodeTests, getBlocksByHashNotInited) {
  VarNote::InProcessNode newNode(coreStub, protocolQueryStub);

  std::vector<Crypto::Hash> blockHashes;
  std::vector<VarNote::BlockDetails> blocks;
  ASSERT_EQ(blockHashes.size(), 0);
  ASSERT_EQ(blocks.size(), 0);

  CallbackStatus status(dispatcher);
  newNode.getBlocks(blockHashes, blocks, [&status] (std::error_code ec) { status.setStatusRemote(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_NE(std::error_code(), status.getStatus());
}
*/

TEST_F(InProcessNodeTests, getTxEmpty) {
  std::vector<Crypto::Hash> transactionHashes;
  std::vector<VarNote::TransactionDetails> transactions;
  ASSERT_EQ(transactionHashes.size(), 0);
  ASSERT_EQ(transactions.size(), 0);

  CallbackStatus status(dispatcher);
  node.getTransactions(transactionHashes, transactions, [&status] (std::error_code ec) { status.setStatusRemote(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_EQ(std::error_code(), status.getStatus());
}

TEST_F(InProcessNodeTests, getTxMany) {
  size_t POOL_TX_NUMBER = 10;
  size_t BLOCKCHAIN_TX_NUMBER = 10;

  std::vector<Crypto::Hash> transactionHashes;
  std::vector<VarNote::TransactionDetails> actualTransactions;

  std::vector<std::tuple<VarNote::Transaction, Crypto::Hash, uint64_t>> expectedTransactions;

  size_t prevBlockchainSize = generator.getBlockchain().size();
  for (size_t i = 0; i < BLOCKCHAIN_TX_NUMBER; ++i) {
    auto txptr = VarNote::createTransaction();
    auto tx = ::createTx(*txptr.get());
    transactionHashes.push_back(VarNote::getObjectHash(tx));
    generator.addTxToBlockchain(tx);

    coreStub.addBlock(generator.getBlockchain().back());
    coreStub.addTransaction(tx);
    expectedTransactions.push_back(std::make_tuple(tx, VarNote::CachedBlock(generator.getBlockchain().back()).getBlockHash(), boost::get<VarNote::BaseInput>(generator.getBlockchain().back().baseTransaction.inputs.front()).blockIndex));
  }

  for (size_t i = 0; i < POOL_TX_NUMBER; ++i) {
    auto txptr = VarNote::createTransaction();
    auto tx = ::createTx(*txptr.get());
    transactionHashes.push_back(VarNote::getObjectHash(tx));
    coreStub.addTransactionToPool(VarNote::toBinaryArray(tx));
    expectedTransactions.push_back(std::make_tuple(tx, boost::value_initialized<Crypto::Hash>(), boost::value_initialized<uint64_t>()));
  }

  CallbackStatus status(dispatcher);
  node.getTransactions(transactionHashes, actualTransactions, [&status] (std::error_code ec) { status.setStatusRemote(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_EQ(std::error_code(), status.getStatus());

  ASSERT_EQ(transactionHashes.size(), expectedTransactions.size());
  ASSERT_EQ(transactionHashes.size(), actualTransactions.size());
  auto range1 = boost::combine(transactionHashes, actualTransactions);
  auto range = boost::combine(range1, expectedTransactions);
  for (const boost::tuple<boost::tuple<Crypto::Hash, VarNote::TransactionDetails>, std::tuple<VarNote::Transaction, Crypto::Hash, uint64_t>>& sameHeight : range) {
    Crypto::Hash expectedCryptoHash = VarNote::getObjectHash(std::get<0>(sameHeight.get<1>()));
    EXPECT_EQ(expectedCryptoHash, sameHeight.get<0>().get<0>());
    Hash expectedHash = reinterpret_cast<const Hash&>(expectedCryptoHash);
    EXPECT_EQ(sameHeight.get<0>().get<1>().hash, expectedHash);
    if (std::get<1>(sameHeight.get<1>()) != boost::value_initialized<Crypto::Hash>()) {
      EXPECT_TRUE(sameHeight.get<0>().get<1>().inBlockchain);
      Hash expectedBlockHash = reinterpret_cast<const Hash&>(std::get<1>(sameHeight.get<1>()));
      EXPECT_EQ(sameHeight.get<0>().get<1>().blockHash, expectedBlockHash);
      EXPECT_EQ(sameHeight.get<0>().get<1>().blockIndex, std::get<2>(sameHeight.get<1>()));
    } else {
      EXPECT_FALSE(sameHeight.get<0>().get<1>().inBlockchain);
    }
  }
}

TEST_F(InProcessNodeTests, getTxFail) {
  size_t POOL_TX_NUMBER = 10;
  size_t BLOCKCHAIN_TX_NUMBER = 10;

  std::vector<Crypto::Hash> transactionHashes;
  std::vector<VarNote::TransactionDetails> actualTransactions;

  std::vector<std::tuple<VarNote::Transaction, Crypto::Hash, uint64_t>> expectedTransactions;

  size_t prevBlockchainSize = generator.getBlockchain().size();
  for (size_t i = 0; i < BLOCKCHAIN_TX_NUMBER; ++i) {
    auto txptr = VarNote::createTransaction();
    auto tx = ::createTx(*txptr.get());
    transactionHashes.push_back(VarNote::getObjectHash(tx));
    generator.addTxToBlockchain(tx);
    ASSERT_EQ(generator.getBlockchain().size(), prevBlockchainSize + 1);
    prevBlockchainSize = generator.getBlockchain().size();
    coreStub.addBlock(generator.getBlockchain().back());
    coreStub.addTransaction(tx);
    expectedTransactions.push_back(std::make_tuple(tx, VarNote::CachedBlock(generator.getBlockchain().back()).getBlockHash(), boost::get<VarNote::BaseInput>(generator.getBlockchain().back().baseTransaction.inputs.front()).blockIndex));
  }

  ASSERT_EQ(transactionHashes.size(), BLOCKCHAIN_TX_NUMBER);
  ASSERT_EQ(transactionHashes.size(), expectedTransactions.size());
  ASSERT_EQ(actualTransactions.size(), 0);

  for (size_t i = 0; i < POOL_TX_NUMBER; ++i) {
    auto txptr = VarNote::createTransaction();
    auto tx = ::createTx(*txptr.get());
    transactionHashes.push_back(VarNote::getObjectHash(tx));
    expectedTransactions.push_back(std::make_tuple(tx, boost::value_initialized<Crypto::Hash>(), boost::value_initialized<uint64_t>()));
  }

  ASSERT_EQ(transactionHashes.size(), BLOCKCHAIN_TX_NUMBER + POOL_TX_NUMBER);
  ASSERT_EQ(transactionHashes.size(), expectedTransactions.size());
  ASSERT_EQ(actualTransactions.size(), 0);


  CallbackStatus status(dispatcher);
  node.getTransactions(transactionHashes, actualTransactions, [&status] (std::error_code ec) { status.setStatusRemote(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_NE(std::error_code(), status.getStatus());

}

TEST_F(InProcessNodeTests, getTxNotInited) {
  VarNote::InProcessNode newNode(coreStub, protocolQueryStub, dispatcher);

  std::vector<Crypto::Hash> transactionHashes;
  std::vector<VarNote::TransactionDetails> transactions;
  ASSERT_EQ(transactionHashes.size(), 0);
  ASSERT_EQ(transactions.size(), 0);

  CallbackStatus status(dispatcher);
  newNode.getTransactions(transactionHashes, transactions, [&status] (std::error_code ec) { status.setStatus(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_NE(std::error_code(), status.getStatus());
}

TEST_F(InProcessNodeTests, isSynchronized) {
  bool syncStatus;
  {
    CallbackStatus status(dispatcher);
    node.isSynchronized(syncStatus, [&status] (std::error_code ec) { status.setStatusRemote(ec); });
    ASSERT_TRUE(status.wait());
    ASSERT_EQ(std::error_code(), status.getStatus());
    ASSERT_FALSE(syncStatus);
  }

  protocolQueryStub.setSynchronizedStatus(true);

  {
    CallbackStatus status(dispatcher);
    node.isSynchronized(syncStatus, [&status] (std::error_code ec) { status.setStatusRemote(ec); });
    ASSERT_TRUE(status.wait());
    ASSERT_EQ(std::error_code(), status.getStatus());
    ASSERT_TRUE(syncStatus);
  }
}

TEST_F(InProcessNodeTests, isSynchronizedNotInited) {
  VarNote::InProcessNode newNode(coreStub, protocolQueryStub, dispatcher);
  bool syncStatus;

  CallbackStatus status(dispatcher);
  newNode.isSynchronized(syncStatus, [&status] (std::error_code ec) { status.setStatus(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_NE(std::error_code(), status.getStatus());
}

TEST_F(InProcessNodeTests, getLastLocalBlockTimestamp) {
  class GetBlockTimestampCore : public ICoreStub {
  public:
    GetBlockTimestampCore(uint64_t timestamp) : timestamp(timestamp) {}

    virtual VarNote::BlockTemplate getBlockByIndex(uint32_t index) const override {
      VarNote::BlockTemplate blk;
      blk.timestamp = timestamp;
      return blk;
    }

    virtual bool addMessageQueue(VarNote::MessageQueue<VarNote::BlockchainMessage>&  messageQueue) override {
      ICoreStub::addMessageQueue(messageQueue);
      messageQueue.push(BlockchainMessage{VarNote::Messages::NewBlock{1, Crypto::Hash()}});
      return true;
    }

    uint64_t timestamp;
  };

  uint64_t expectedTimestamp = 1234567890;
  GetBlockTimestampCore core(expectedTimestamp);
  VarNote::InProcessNode newNode(core, protocolQueryStub, dispatcher);

  CallbackStatus initStatus(dispatcher);
  newNode.init([&initStatus] (std::error_code ec) { initStatus.setStatusRemote(ec); });
  ASSERT_TRUE(initStatus.wait());

  System::Timer timer(dispatcher);
  timer.sleep(std::chrono::milliseconds(100)); //bad inproc node architecture

  uint64_t timestamp = invokeNodeMethod<uint64_t>([&newNode] () { return newNode.getLastLocalBlockTimestamp(); });

  ASSERT_EQ(expectedTimestamp, timestamp);
}

TEST_F(InProcessNodeTests, getPoolDiffereceNotInited) {
  VarNote::InProcessNode newNode(coreStub, protocolQueryStub, dispatcher);

  std::vector<Crypto::Hash> knownPoolTxIds;
  Crypto::Hash knownBlockId = boost::value_initialized<Crypto::Hash>();
  bool isBcActual = false;
  std::vector<std::unique_ptr<ITransactionReader>> newTxs;
  std::vector<Crypto::Hash> deletedTxIds;

  CallbackStatus status(dispatcher);
  newNode.getPoolSymmetricDifference(std::move(knownPoolTxIds), knownBlockId, isBcActual, newTxs, deletedTxIds, [&status](std::error_code ec) { status.setStatus(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_NE(std::error_code(), status.getStatus());
}

TEST_F(InProcessNodeTests, getPoolDiffereceActualBC) {
  size_t POOL_TX_NUMBER = 10;

  std::unordered_set<Crypto::Hash> transactionHashes;

  coreStub.setPoolChangesResult(true);

  for (size_t i = 0; i < POOL_TX_NUMBER; ++i) {
    auto txptr = VarNote::createTransaction();
    transactionHashes.insert(txptr->getTransactionHash());
    coreStub.addTransactionToPool(txptr->getTransactionData());
  }

  std::vector<Crypto::Hash> knownPoolTxIds;
  Crypto::Hash knownBlockId = VarNote::getObjectHash(generator.getBlockchain().back());
  bool isBcActual = false;
  std::vector<std::unique_ptr<ITransactionReader>> newTxs;
  std::vector<Crypto::Hash> deletedTxIds;

  CallbackStatus status(dispatcher);
  node.getPoolSymmetricDifference(std::move(knownPoolTxIds), knownBlockId, isBcActual, newTxs, deletedTxIds, [&status](std::error_code ec) { status.setStatusRemote(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_EQ(std::error_code(), status.getStatus());
  ASSERT_TRUE(isBcActual);
  ASSERT_EQ(newTxs.size(), transactionHashes.size());
  ASSERT_TRUE(deletedTxIds.empty());

  for (const auto& tx : newTxs) {
    ASSERT_NE(transactionHashes.find(tx->getTransactionHash()), transactionHashes.end());
  }
}

TEST_F(InProcessNodeTests, getPoolDiffereceNotActualBC) {
  size_t POOL_TX_NUMBER = 10;

  std::unordered_set<Crypto::Hash> transactionHashes;

  coreStub.setPoolChangesResult(false);

  for (size_t i = 0; i < POOL_TX_NUMBER; ++i) {
    auto txptr = VarNote::createTransaction();
    transactionHashes.insert(txptr->getTransactionHash());
    coreStub.addTransactionToPool(txptr->getTransactionData());
  }

  std::vector<Crypto::Hash> knownPoolTxIds;
  Crypto::Hash knownBlockId = VarNote::getObjectHash(generator.getBlockchain().back());
  bool isBcActual = false;
  std::vector<std::unique_ptr<ITransactionReader>> newTxs;
  std::vector<Crypto::Hash> deletedTxIds;

  CallbackStatus status(dispatcher);
  node.getPoolSymmetricDifference(std::move(knownPoolTxIds), knownBlockId, isBcActual, newTxs, deletedTxIds, [&status](std::error_code ec) { status.setStatusRemote(ec); });
  ASSERT_TRUE(status.wait());
  ASSERT_EQ(std::error_code(), status.getStatus());
  ASSERT_FALSE(isBcActual);
  ASSERT_EQ(newTxs.size(), transactionHashes.size());
  ASSERT_TRUE(deletedTxIds.empty());

  for (const auto& tx : newTxs) {
    ASSERT_NE(transactionHashes.find(tx->getTransactionHash()), transactionHashes.end());
  }
}

//TODO: make relayTransaction unit test
//TODO: make getNewBlocks unit test
//TODO: make queryBlocks unit test
