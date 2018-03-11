// Copyright (c) 2012-2018, The CryptoNote developers, YxomTech
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

#pragma once

#include <vector>
#include <unordered_map>

#include "VarNoteCore/Account.h"
#include "VarNoteCore/VarNoteBasic.h"
#include "VarNoteCore/Currency.h"
#include "VarNoteCore/BlockchainIndices.h"
#include "crypto/hash.h"

#include "../TestGenerator/TestGenerator.h"

class TestBlockchainGenerator
{
public:
  TestBlockchainGenerator(const VarNote::Currency& currency);

  //TODO: get rid of this method
  std::vector<VarNote::Block>& getBlockchain();
  std::vector<VarNote::Block> getBlockchainCopy();
  void generateEmptyBlocks(uint32_t count);
  bool getBlockRewardForAddress(const VarNote::AccountPublicAddress& address);
  bool generateTransactionsInOneBlock(const VarNote::AccountPublicAddress& address, size_t n);
  bool getSingleOutputTransaction(const VarNote::AccountPublicAddress& address, uint64_t amount);
  void addTxToBlockchain(const VarNote::Transaction& transaction);
  bool getTransactionByHash(const Crypto::Hash& hash, VarNote::Transaction& tx, bool checkTxPool = false);
  const VarNote::AccountBase& getMinerAccount() const;

  void putTxToPool(const VarNote::Transaction& tx);
  void getPoolSymmetricDifference(std::vector<Crypto::Hash>&& known_pool_tx_ids, Crypto::Hash known_block_id, bool& is_bc_actual,
    std::vector<VarNote::Transaction>& new_txs, std::vector<Crypto::Hash>& deleted_tx_ids);
  void putTxPoolToBlockchain();
  void clearTxPool();

  void cutBlockchain(uint32_t height);

  bool addOrphan(const Crypto::Hash& hash, uint32_t height);
  bool getGeneratedTransactionsNumber(uint32_t height, uint64_t& generatedTransactions);
  bool getOrphanBlockIdsByHeight(uint32_t height, std::vector<Crypto::Hash>& blockHashes);
  bool getBlockIdsByTimestamp(uint64_t timestampBegin, uint64_t timestampEnd, uint32_t blocksNumberLimit, std::vector<Crypto::Hash>& hashes, uint32_t& blocksNumberWithinTimestamps);
  bool getPoolTransactionIdsByTimestamp(uint64_t timestampBegin, uint64_t timestampEnd, uint32_t transactionsNumberLimit, std::vector<Crypto::Hash>& hashes, uint64_t& transactionsNumberWithinTimestamps);
  bool getTransactionIdsByPaymentId(const Crypto::Hash& paymentId, std::vector<Crypto::Hash>& transactionHashes);

  bool getTransactionGlobalIndexesByHash(const Crypto::Hash& transactionHash, std::vector<uint32_t>& globalIndexes);
  bool getMultisignatureOutputByGlobalIndex(uint64_t amount, uint32_t globalIndex, VarNote::MultisignatureOutput& out);
  void setMinerAccount(const VarNote::AccountBase& account);

private:
  struct MultisignatureOutEntry {
    Crypto::Hash transactionHash;
    uint16_t indexOut;
  };

  struct KeyOutEntry {
    Crypto::Hash transactionHash;
    uint16_t indexOut;
  };
  
  void addGenesisBlock();
  void addMiningBlock();

  const VarNote::Currency& m_currency;
  test_generator generator;
  VarNote::AccountBase miner_acc;
  std::vector<VarNote::Block> m_blockchain;
  std::unordered_map<Crypto::Hash, VarNote::Transaction> m_txs;
  std::unordered_map<Crypto::Hash, std::vector<uint32_t>> transactionGlobalOuts;
  std::unordered_map<uint64_t, std::vector<MultisignatureOutEntry>> multisignatureOutsIndex;
  std::unordered_map<uint64_t, std::vector<KeyOutEntry>> keyOutsIndex;

  std::unordered_map<Crypto::Hash, VarNote::Transaction> m_txPool;
  mutable std::mutex m_mutex;

  VarNote::PaymentIdIndex m_paymentIdIndex;
  VarNote::TimestampTransactionsIndex m_timestampIndex;
  VarNote::GeneratedTransactionsIndex m_generatedTransactionsIndex;
  VarNote::OrphanBlocksIndex m_orthanBlocksIndex;

  void addToBlockchain(const VarNote::Transaction& tx);
  void addToBlockchain(const std::vector<VarNote::Transaction>& txs);
  void addTx(const VarNote::Transaction& tx);

  bool doGenerateTransactionsInOneBlock(VarNote::AccountPublicAddress const &address, size_t n);
};
