// Copyright (c) 2012-2013 The VarNote developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <cstdint>
#include <functional>
#include <system_error>
#include <vector>

#include "crypto/crypto.h"
#include "VarNote_core/VarNote_basic.h"
#include "VarNote_protocol/VarNote_protocol_defs.h"
#include "rpc/core_rpc_server_commands_defs.h"

namespace VarNote {

class INodeObserver {
public:
  virtual ~INodeObserver() {}
  virtual void peerCountUpdated(size_t count) {}
  virtual void localBlockchainUpdated(uint64_t height) {}
  virtual void lastKnownBlockHeightUpdated(uint64_t height) {}
};

struct OutEntry {
  uint64_t outGlobalIndex;
  crypto::public_key outKey;
};

struct OutsForAmount {
  uint64_t amount;
  std::vector<OutEntry> outs;
};

class INode {
public:
  typedef std::function<void(std::error_code)> Callback;

  virtual ~INode() {}
  virtual bool addObserver(INodeObserver* observer) = 0;
  virtual bool removeObserver(INodeObserver* observer) = 0;

  virtual void init(const Callback& callback) = 0;
  virtual bool shutdown() = 0;

  virtual size_t getPeerCount() const = 0;
  virtual uint64_t getLastLocalBlockHeight() const = 0;
  virtual uint64_t getLastKnownBlockHeight() const = 0;

  virtual void relayTransaction(const VarNote::transaction& transaction, const Callback& callback) = 0;
  virtual void getRandomOutsByAmounts(std::vector<uint64_t>&& amounts, uint64_t outsCount, std::vector<VarNote::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::outs_for_amount>& result, const Callback& callback) = 0;
  virtual void getNewBlocks(std::list<crypto::hash>&& knownBlockIds, std::list<VarNote::block_complete_entry>& newBlocks, uint64_t& startHeight, const Callback& callback) = 0;
  virtual void getTransactionOutsGlobalIndices(const crypto::hash& transactionHash, std::vector<uint64_t>& outsGlobalIndices, const Callback& callback) = 0;
};

}
