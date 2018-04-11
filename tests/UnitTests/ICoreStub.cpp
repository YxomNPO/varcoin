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

#include "ICoreStub.h"

#include "VarNoteCore/VarNoteFormatUtils.h"
#include "VarNoteCore/VarNoteTools.h"
#include "VarNoteCore/VerificationContext.h"
#include "VarNoteCore/TransactionApi.h"


void notifyObservers(VarNote::BlockchainMessage&& msg,
                     VarNote::IntrusiveLinkedList<VarNote::MessageQueue<VarNote::BlockchainMessage>>& queueList)  {
  for (auto& queue : queueList) {
    queue.push(std::move(msg));
  }
}

ICoreStub::ICoreStub() :
    topHeight(0),
    globalIndicesResult(false),
    randomOutsResult(false),
    poolTxVerificationResult(true),
    poolChangesResult(true) {
}

ICoreStub::ICoreStub(const VarNote::BlockTemplate& genesisBlock) :
    topHeight(0),
    globalIndicesResult(false),
    randomOutsResult(false),
    poolTxVerificationResult(true),
    poolChangesResult(true) {
  addBlock(genesisBlock);
}

bool ICoreStub::addObserver(VarNote::ICoreObserver* observer) {
  return m_observerManager.add(observer);
}

bool ICoreStub::removeObserver(VarNote::ICoreObserver* observer) {
  return m_observerManager.remove(observer);
}

std::vector<Crypto::Hash> ICoreStub::findBlockchainSupplement(const std::vector<Crypto::Hash>& remoteBlockIds, size_t maxCount,
  uint32_t& totalBlockCount, uint32_t& startBlockIndex) const {

  //Sending all blockchain
  totalBlockCount = static_cast<uint32_t>(blocks.size());
  startBlockIndex = 0;
  std::vector<Crypto::Hash> result;
  result.reserve(std::min(blocks.size(), maxCount));
  for (uint32_t height = 0; height < static_cast<uint32_t>(std::min(blocks.size(), maxCount)); ++height) {
    assert(blockHashByHeightIndex.count(height) > 0);
    result.push_back(blockHashByHeightIndex.at(height));
  }
  return result;
}

void ICoreStub::set_blockchain_top(uint32_t height, const Crypto::Hash& top_id) {
  topHeight = height;
  topId = top_id;
  m_observerManager.notify(&VarNote::ICoreObserver::blockchainUpdated);
}

void ICoreStub::set_outputs_gindexs(const std::vector<uint32_t>& indexs, bool result) {
  globalIndices.clear();
  std::copy(indexs.begin(), indexs.end(), std::back_inserter(globalIndices));
  globalIndicesResult = result;
}

void ICoreStub::set_random_outs(const VarNote::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_response& resp, bool result) {
  randomOuts = resp;
  randomOutsResult = result;
}

bool ICoreStub::getPoolChanges(const Crypto::Hash& tailBlockId, const std::vector<Crypto::Hash>& knownTxsIds, std::vector<VarNote::BinaryArray>& addedTxs, std::vector<Crypto::Hash>& deletedTxsIds) const {
  std::unordered_set<Crypto::Hash> knownSet;
  for (const Crypto::Hash& txId : knownTxsIds) {
    if (transactionPool.find(txId) == transactionPool.end()) {
      deletedTxsIds.push_back(txId);
    }

    knownSet.insert(txId);
  }

  for (const std::pair<Crypto::Hash, VarNote::BinaryArray>& poolEntry : transactionPool) {
    if (knownSet.find(poolEntry.first) == knownSet.end()) {
      addedTxs.push_back(poolEntry.second);
    }
  }

  return poolChangesResult;
}

bool ICoreStub::getPoolChangesLite(const Crypto::Hash& tailBlockId, const std::vector<Crypto::Hash>& knownTxsIds,
          std::vector<VarNote::TransactionPrefixInfo>& addedTxs, std::vector<Crypto::Hash>& deletedTxsIds) const {
  std::vector<VarNote::BinaryArray> added;
  bool returnStatus = getPoolChanges(tailBlockId, knownTxsIds, added, deletedTxsIds);

  for (const auto& txBlob : added) {
    VarNote::Transaction tx;
    if (!fromBinaryArray(tx, txBlob)) {
      assert(false);
    }

    VarNote::TransactionPrefixInfo tpi;
    tpi.txPrefix = tx;
    tpi.txHash = getObjectHash(tx);

    addedTxs.push_back(std::move(tpi));
  }

  return returnStatus;
}

bool ICoreStub::queryBlocks(const std::vector<Crypto::Hash>& block_ids, uint64_t timestamp,
    uint32_t& start_height, uint32_t& current_height, uint32_t& full_offset, std::vector<VarNote::BlockFullInfo>& entries) const {
  //stub
  return true;
}

bool ICoreStub::queryBlocksLite(const std::vector<Crypto::Hash>& block_ids, uint64_t timestamp,
    uint32_t& start_height, uint32_t& current_height, uint32_t& full_offset, std::vector<VarNote::BlockShortInfo>& entries) const {
  //stub
  return true;
}

std::vector<Crypto::Hash> ICoreStub::buildSparseChain() const {
  std::vector<Crypto::Hash> result;
  result.reserve(blockHashByHeightIndex.size());
  for (auto kvPair : blockHashByHeightIndex) {
    result.emplace_back(kvPair.second);
  }

  std::reverse(result.begin(), result.end());
  return result;
}

VarNote::BlockTemplate ICoreStub::getBlockByIndex(uint32_t height) const {
  return blocks.at(blockHashByHeightIndex.at(height));
}
  
uint64_t ICoreStub::getBlockTimestampByIndex(uint32_t blockIndex) const {
  return getBlockByIndex(blockIndex).timestamp;
}

VarNote::BlockTemplate ICoreStub::getBlockByHash(const Crypto::Hash &h) const {
  auto iter = blocks.find(h);
  if (iter == blocks.end()) {
    throw std::logic_error("no such block");
  }
  return iter->second;
}
  
Crypto::Hash ICoreStub::getBlockHashByIndex(uint32_t height) const {
  auto block = getBlockByIndex(height);
  return VarNote::CachedBlock(block).getBlockHash();
}
  
bool ICoreStub::addMessageQueue(MessageQueue<BlockchainMessage>& messageQueue) {
  return queueList.insert(messageQueue);
}

bool ICoreStub::removeMessageQueue(MessageQueue<BlockchainMessage>& messageQueue) {
  return queueList.remove(messageQueue);
}

uint32_t ICoreStub::getTopBlockIndex() const {
  return topHeight;
}
  
Crypto::Hash ICoreStub::getTopBlockHash() const {
  return topId;
}

void ICoreStub::getTransactions(const std::vector<Crypto::Hash>& txs_ids, std::vector<VarNote::BinaryArray>& txs,
                                std::vector<Crypto::Hash>& missed_txs) const {
  for (const Crypto::Hash& hash : txs_ids) {
    auto iter = transactions.find(hash);
    if (iter != transactions.end()) {
      txs.push_back(iter->second);
    } else {
      missed_txs.push_back(hash);
    }
  }
  auto pullTxIds = std::move(missed_txs);
  for (const Crypto::Hash& hash : pullTxIds) {
    auto iter = transactionPool.find(hash);
    if (iter != transactionPool.end()) {
      txs.push_back(iter->second);
    } else {
      missed_txs.push_back(hash);
    }
  }
}

VarNote::Difficulty ICoreStub::getBlockDifficulty(uint32_t height) const {
  //TODO: implement it
  return 1;
}

void ICoreStub::addBlock(const VarNote::BlockTemplate& block) {
  uint32_t height = boost::get<VarNote::BaseInput>(block.baseTransaction.inputs.front()).blockIndex;
  auto hash = VarNote::CachedBlock(block).getBlockHash();
  if (height > topHeight || blocks.empty()) {
    topHeight = height;
    topId = hash;
  }
  blocks.emplace(std::make_pair(hash, block));
  blockHashByHeightIndex.emplace(std::make_pair(height, hash));
  blockHeightByHashIndex.emplace(hash, height);

  blockHashByTxHashIndex.emplace(std::make_pair(VarNote::getObjectHash(block.baseTransaction), hash));
  for (auto txHash : block.transactionHashes) {
    blockHashByTxHashIndex.emplace(std::make_pair(txHash, hash));
  }

  notifyObservers(BlockchainMessage{VarNote::Messages::NewBlock{topHeight, topId}}, queueList);
  m_observerManager.notify(&VarNote::ICoreObserver::blockchainUpdated);
}

void ICoreStub::addTransaction(const VarNote::Transaction& tx) {
  Crypto::Hash hash = VarNote::getObjectHash(tx);
  transactions.emplace(hash, VarNote::toBinaryArray(tx));
}

std::vector<VarNote::RawBlock> ICoreStub::getBlocks(uint32_t startIndex, uint32_t count) const {
  //TODO:
  assert(false);
  return {};
}

void ICoreStub::getBlocks(const std::vector<Crypto::Hash>& blockHashes, std::vector<VarNote::RawBlock>& blocks, std::vector<Crypto::Hash>& missedHashes) const {
  //TODO:
  assert(false);
}
  
std::error_code ICoreStub::submitBlock(VarNote::BinaryArray&& rawBlockTemplate) {
  assert(false);
  return {};
}
  
bool ICoreStub::getTransactionGlobalIndexes(const Crypto::Hash& transactionHash, std::vector<uint32_t>& globalIndexes) const {
  globalIndexes = globalIndices;
  return globalIndicesResult;
}

bool ICoreStub::getRandomOutputs(uint64_t amount, uint16_t count, std::vector<uint32_t>& globalIndexes, std::vector<Crypto::PublicKey>& publicKeys) const {
  bool found = false;

  for (const VarNote::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_outs_for_amount& out: randomOuts.outs) {
    if (out.amount != amount) {
      continue;
    }

    for (size_t i = 0; i < count && i < out.outs.size(); ++i) {
      globalIndexes.push_back(out.outs[i].global_amount_index);
      publicKeys.push_back(out.outs[i].out_key);
    }

    found = true;
  }

  if (!found) {
    throw std::runtime_error("requested amount is not found");
  }

  return randomOutsResult;
}

bool ICoreStub::addTransactionToPool(const VarNote::BinaryArray& transactionBinaryArray) {
  transactionPool.emplace(VarNote::getBinaryArrayHash(transactionBinaryArray), transactionBinaryArray);
  return true;
}

std::vector<Crypto::Hash> ICoreStub::getPoolTransactionHashes() const {
  assert(false);
  return {};
}

bool ICoreStub::getBlockTemplate(VarNote::BlockTemplate& b, const VarNote::AccountPublicAddress& adr, const VarNote::BinaryArray& extraNonce, VarNote::Difficulty& difficulty, uint32_t& height) const {
  assert(false);
  return false;
}

VarNote::CoreStatistics ICoreStub::getCoreStatistics() const {
  assert(false);
  return {};
}

void ICoreStub::save() {
  assert(false);
}

void ICoreStub::load() {
  assert(false);
}

VarNote::Difficulty ICoreStub::getDifficultyForNextBlock() const {
  assert(false);
  return 0;
}
  
std::error_code ICoreStub::addBlock(const VarNote::CachedBlock& cachedBlock, VarNote::RawBlock&& rawBlock) {
  assert(false);
  return {};
}

std::error_code ICoreStub::addBlock(VarNote::RawBlock&& rawBlock) {
  assert(false);
  return {};
}

bool ICoreStub::hasBlock(const Crypto::Hash& id) const {
  return blocks.count(id) > 0;
}

void ICoreStub::setPoolTxVerificationResult(bool result) {
  poolTxVerificationResult = result;
}

bool ICoreStub::hasTransaction(const Crypto::Hash& transactionHash) const {
  return transactions.find(transactionHash) != transactions.end() || transactionPool.find(transactionHash) != transactionPool.end();
}

VarNote::BlockDetails ICoreStub::getBlockDetails(const Crypto::Hash& blockHash) const {
  VarNote::BlockDetails details;

  VarNote::BlockTemplate blockTemplate = blocks.at(blockHash);

  details.majorVersion = blockTemplate.majorVersion;
  details.minorVersion = blockTemplate.minorVersion;
  details.timestamp = blockTemplate.timestamp;
  details.prevBlockHash = blockTemplate.previousBlockHash;
  details.nonce = blockTemplate.nonce;
  details.hash = blockHash;
  details.index = blockHeightByHashIndex.at(blockHash);

  return details;
}

VarNote::TransactionDetails ICoreStub::getTransactionDetails(const Crypto::Hash& transactionHash) const {
  VarNote::BinaryArray transactionBinaryArray;

  bool foundInPool = false;
  auto bcIt = transactions.find(transactionHash);
  if (bcIt == transactions.end()) {
    auto poolIt = transactionPool.find(transactionHash);
    if (poolIt == transactionPool.end()) {
      throw std::runtime_error("transaction not found");
    }

    transactionBinaryArray = poolIt->second;
    foundInPool = true;
  } else {
    transactionBinaryArray = bcIt->second;
  }

  auto transaction = VarNote::createTransaction(transactionBinaryArray);

  VarNote::TransactionDetails transactionDetails;
  transactionDetails.hash = transactionHash;
  transactionDetails.size = transactionBinaryArray.size();
  transactionDetails.totalInputsAmount = transaction->getInputTotalAmount();
  transactionDetails.totalOutputsAmount = transaction->getOutputTotalAmount();
  transactionDetails.fee = transactionDetails.totalOutputsAmount - transactionDetails.totalInputsAmount;
  transactionDetails.unlockTime = transaction->getUnlockTime();
  transactionDetails.hasPaymentId = transaction->getPaymentId(transactionDetails.paymentId);
  transactionDetails.inBlockchain = !foundInPool;

  if (transactionDetails.inBlockchain) {
    transactionDetails.blockHash = blockHashByTxHashIndex.at(transactionHash);
    transactionDetails.blockIndex = blockHeightByHashIndex.at(blockHashByTxHashIndex.at(transactionHash));
  }

  return transactionDetails;
}

std::vector<Crypto::Hash> ICoreStub::getAlternativeBlockHashesByIndex(uint32_t blockIndex) const {
  return std::vector<Crypto::Hash>();
}

void ICoreStub::setPoolChangesResult(bool result) {
  poolChangesResult = result;
}
