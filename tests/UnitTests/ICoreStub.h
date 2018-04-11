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

#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include <boost/bimap.hpp>

#include "Common/ObserverManager.h"
#include "VarNoteCore/VarNoteBasic.h"
#include "VarNoteCore/ICore.h"
#include "VarNoteCore/ICoreObserver.h"
#include "VarNoteCore/IntrusiveLinkedList.h"
#include "VarNoteCore/MessageQueue.h"
#include "VarNoteCore/BlockchainMessages.h"
#include "VarNoteProtocol/VarNoteProtocolDefinitions.h"
#include "Rpc/CoreRpcServerCommandsDefinitions.h"

class ICoreStub: public VarNote::ICore {
public:
  ICoreStub();
  ICoreStub(const VarNote::BlockTemplate& genesisBlock);

  template <class T> using MessageQueue = VarNote::MessageQueue<T>;
  using BlockchainMessage = VarNote::BlockchainMessage;

  virtual bool addMessageQueue(MessageQueue<BlockchainMessage>&  messageQueue) override;
  virtual bool removeMessageQueue(MessageQueue<BlockchainMessage>& messageQueue) override;
  virtual uint32_t getTopBlockIndex() const override;
  virtual Crypto::Hash getTopBlockHash() const override;
  virtual uint64_t getBlockTimestampByIndex(uint32_t blockIndex) const override;
  virtual VarNote::BlockTemplate getBlockByIndex(uint32_t index) const override;
  
  virtual VarNote::Difficulty getDifficultyForNextBlock() const override;
  virtual std::error_code addBlock(const VarNote::CachedBlock& cachedBlock, VarNote::RawBlock&& rawBlock) override;
  virtual std::error_code addBlock(VarNote::RawBlock&& rawBlock) override;
  virtual std::error_code submitBlock(VarNote::BinaryArray&& rawBlockTemplate) override;
  
  virtual std::vector<VarNote::RawBlock> getBlocks(uint32_t startIndex, uint32_t count) const override;
  virtual void getBlocks(const std::vector<Crypto::Hash>& blockHashes, std::vector<VarNote::RawBlock>& blocks, std::vector<Crypto::Hash>& missedHashes) const override;
  virtual bool getRandomOutputs(uint64_t amount, uint16_t count, std::vector<uint32_t>& globalIndexes, std::vector<Crypto::PublicKey>& publicKeys) const override;
  virtual bool addTransactionToPool(const VarNote::BinaryArray& transactionBinaryArray) override;
  virtual std::vector<Crypto::Hash> getPoolTransactionHashes() const override;
  virtual bool getBlockTemplate(VarNote::BlockTemplate& b, const VarNote::AccountPublicAddress& adr, const VarNote::BinaryArray& extraNonce, VarNote::Difficulty& difficulty, uint32_t& height) const override;

  virtual VarNote::CoreStatistics getCoreStatistics() const override;

  virtual void save() override;
  virtual void load() override;

  virtual std::vector<Crypto::Hash> findBlockchainSupplement(const std::vector<Crypto::Hash>& remoteBlockIds, size_t maxCount,
    uint32_t& totalBlockCount, uint32_t& startBlockIndex) const override;
  virtual bool getPoolChanges(const Crypto::Hash& tailBlockId, const std::vector<Crypto::Hash>& knownTxsIds, std::vector<VarNote::BinaryArray>& addedTxs, std::vector<Crypto::Hash>& deletedTxsIds) const override;
  virtual bool getPoolChangesLite(const Crypto::Hash& tailBlockId, const std::vector<Crypto::Hash>& knownTxsIds,
          std::vector<VarNote::TransactionPrefixInfo>& addedTxs, std::vector<Crypto::Hash>& deletedTxsIds) const override;
  virtual bool queryBlocks(const std::vector<Crypto::Hash>& block_ids, uint64_t timestamp,
    uint32_t& start_height, uint32_t& current_height, uint32_t& full_offset, std::vector<VarNote::BlockFullInfo>& entries) const override;
  virtual bool queryBlocksLite(const std::vector<Crypto::Hash>& block_ids, uint64_t timestamp,
    uint32_t& start_height, uint32_t& current_height, uint32_t& full_offset, std::vector<VarNote::BlockShortInfo>& entries) const override;

  virtual bool hasBlock(const Crypto::Hash& id) const override;
  std::vector<Crypto::Hash> buildSparseChain() const override;
  virtual bool getTransactionGlobalIndexes(const Crypto::Hash& transactionHash, std::vector<uint32_t>& globalIndexes) const override;

  virtual Crypto::Hash getBlockHashByIndex(uint32_t height) const override;
  virtual VarNote::BlockTemplate getBlockByHash(const Crypto::Hash &h) const override;
  virtual void getTransactions(const std::vector<Crypto::Hash>& txs_ids, std::vector<VarNote::BinaryArray>& txs, std::vector<Crypto::Hash>& missed_txs) const override;
  virtual VarNote::Difficulty getBlockDifficulty(uint32_t index) const override;


  bool addObserver(VarNote::ICoreObserver* observer);
  bool removeObserver(VarNote::ICoreObserver* observer);
  void set_blockchain_top(uint32_t height, const Crypto::Hash& top_id);
  void set_outputs_gindexs(const std::vector<uint32_t>& indexs, bool result);
  void set_random_outs(const VarNote::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_response& resp, bool result);

  void addBlock(const VarNote::BlockTemplate& block);
  void addTransaction(const VarNote::Transaction& tx);

  void setPoolTxVerificationResult(bool result);
  void setPoolChangesResult(bool result);
  boost::optional<std::pair<VarNote::MultisignatureOutput, uint64_t>>
  getMultisignatureOutput(uint64_t amount, uint32_t globalIndex) const override { return {}; }

  virtual bool hasTransaction(const Crypto::Hash& transactionHash) const override;
  virtual VarNote::BlockDetails getBlockDetails(const Crypto::Hash& blockHash) const override;
  virtual VarNote::TransactionDetails getTransactionDetails(const Crypto::Hash& transactionHash) const override;
  virtual std::vector<Crypto::Hash> getAlternativeBlockHashesByIndex(uint32_t blockIndex) const override;
  virtual std::vector<Crypto::Hash> getBlockHashesByTimestamps(uint64_t timestampBegin, size_t secondsCount) const override { return {};}
  virtual std::vector<Crypto::Hash> getTransactionHashesByPaymentId(const Crypto::Hash& paymentId) const override { return {}; }

private:
  uint32_t topHeight;
  Crypto::Hash topId;

  std::vector<uint32_t> globalIndices;
  bool globalIndicesResult;

  VarNote::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_response randomOuts;
  bool randomOutsResult;

  std::unordered_map<Crypto::Hash, VarNote::BlockTemplate> blocks;
  std::unordered_map<uint32_t, Crypto::Hash> blockHashByHeightIndex; //TODO: replace these two indexes with boost bimap
  std::unordered_map<Crypto::Hash, uint32_t> blockHeightByHashIndex;
  std::unordered_map<Crypto::Hash, Crypto::Hash> blockHashByTxHashIndex;

  std::unordered_map<Crypto::Hash, VarNote::BinaryArray> transactions;
  std::unordered_map<Crypto::Hash, VarNote::BinaryArray> transactionPool;
  bool poolTxVerificationResult;
  bool poolChangesResult;
  std::unordered_map<Crypto::Hash, Crypto::Hash> transactionBlockHashes;
  Tools::ObserverManager<VarNote::ICoreObserver> m_observerManager;

  VarNote::IntrusiveLinkedList<MessageQueue<BlockchainMessage>> queueList;
};
