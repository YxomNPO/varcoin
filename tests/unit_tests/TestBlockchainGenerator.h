// Copyright (c) 2012-2013 The VarNote developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "../core_tests/chaingen.h"
#include <vector>
#include <unordered_map>

#include "VarNote_core/account.h"
#include "VarNote_core/VarNote_basic.h"
#include "crypto/hash.h"

class TestBlockchainGenerator
{
public:
  TestBlockchainGenerator();

  std::vector<VarNote::block>& getBlockchain();
  void addGenesisBlock();
  void generateEmptyBlocks(size_t count);
  bool getBlockRewardForAddress(const VarNote::account_public_address& address);
  void addTxToBlockchain(const VarNote::transaction& transaction);
  bool getTransactionByHash(const crypto::hash& hash, VarNote::transaction& tx);

private:
  test_generator generator;
  VarNote::account_base miner_acc;
  std::vector<VarNote::block> m_blockchain;
  std::unordered_map<crypto::hash, VarNote::transaction> m_txs;
};
