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

#include <cstdint>
#include <list>
#include <vector>
#include <unordered_map>

#include "crypto/hash.h"
#include "VarNoteCore/VarNoteBasic.h"
#include "VarNoteCore/VarNoteBasicImpl.h"
#include "VarNoteCore/VarNoteFormatUtils.h"
#include "VarNoteCore/Currency.h"
#include "VarNoteCore/Difficulty.h"


class test_generator
{
public:
  struct BlockInfo {
    BlockInfo()
      : previousBlockHash()
      , alreadyGeneratedCoins(0)
      , blockSize(0) {
    }

    BlockInfo(Crypto::Hash aPrevId, uint64_t anAlreadyGeneratedCoins, size_t aBlockSize)
      : previousBlockHash(aPrevId)
      , alreadyGeneratedCoins(anAlreadyGeneratedCoins)
      , blockSize(aBlockSize) {
    }

    Crypto::Hash previousBlockHash;
    uint64_t alreadyGeneratedCoins;
    size_t blockSize;
  };

  enum BlockFields {
    bf_none      = 0,
    bf_major_ver = 1 << 0,
    bf_minor_ver = 1 << 1,
    bf_timestamp = 1 << 2,
    bf_prev_id   = 1 << 3,
    bf_miner_tx  = 1 << 4,
    bf_tx_hashes = 1 << 5,
    bf_diffic    = 1 << 6
  };

  test_generator(const VarNote::Currency& currency, uint8_t majorVersion = VarNote::BLOCK_MAJOR_VERSION_1,
                 uint8_t minorVersion = VarNote::BLOCK_MINOR_VERSION_0)
    : m_currency(currency), defaultMajorVersion(majorVersion), defaultMinorVersion(minorVersion) {
  }


  uint8_t defaultMajorVersion;
  uint8_t defaultMinorVersion;

  const VarNote::Currency& currency() const { return m_currency; }

  void getBlockchain(std::vector<BlockInfo>& blockchain, const Crypto::Hash& head, size_t n) const;
  void getLastNBlockSizes(std::vector<size_t>& blockSizes, const Crypto::Hash& head, size_t n) const;
  uint64_t getAlreadyGeneratedCoins(const Crypto::Hash& blockId) const;
  uint64_t getAlreadyGeneratedCoins(const VarNote::Block& blk) const;

  void addBlock(const VarNote::Block& blk, size_t tsxSize, uint64_t fee, std::vector<size_t>& blockSizes,
    uint64_t alreadyGeneratedCoins);
  bool constructBlock(VarNote::Block& blk, uint32_t height, const Crypto::Hash& previousBlockHash,
    const VarNote::AccountBase& minerAcc, uint64_t timestamp, uint64_t alreadyGeneratedCoins,
    std::vector<size_t>& blockSizes, const std::list<VarNote::Transaction>& txList);
  bool constructBlock(VarNote::Block& blk, const VarNote::AccountBase& minerAcc, uint64_t timestamp);
  bool constructBlock(VarNote::Block& blk, const VarNote::Block& blkPrev, const VarNote::AccountBase& minerAcc,
    const std::list<VarNote::Transaction>& txList = std::list<VarNote::Transaction>());

  bool constructBlockManually(VarNote::Block& blk, const VarNote::Block& prevBlock,
    const VarNote::AccountBase& minerAcc, int actualParams = bf_none, uint8_t majorVer = 0,
    uint8_t minorVer = 0, uint64_t timestamp = 0, const Crypto::Hash& previousBlockHash = Crypto::Hash(),
    const VarNote::difficulty_type& diffic = 1, const VarNote::Transaction& baseTransaction = VarNote::Transaction(),
    const std::vector<Crypto::Hash>& transactionHashes = std::vector<Crypto::Hash>(), size_t txsSizes = 0, uint64_t fee = 0);
  bool constructBlockManuallyTx(VarNote::Block& blk, const VarNote::Block& prevBlock,
    const VarNote::AccountBase& minerAcc, const std::vector<Crypto::Hash>& transactionHashes, size_t txsSize);
  bool constructMaxSizeBlock(VarNote::Block& blk, const VarNote::Block& blkPrev,
    const VarNote::AccountBase& minerAccount, size_t medianBlockCount = 0,
    const std::list<VarNote::Transaction>& txList = std::list<VarNote::Transaction>());

private:
  const VarNote::Currency& m_currency;
  std::unordered_map<Crypto::Hash, BlockInfo> m_blocksInfo;
};

inline VarNote::difficulty_type getTestDifficulty() { return 1; }
void fillNonce(VarNote::Block& blk, const VarNote::difficulty_type& diffic);

bool constructMinerTxManually(const VarNote::Currency& currency, uint32_t height, uint64_t alreadyGeneratedCoins,
  const VarNote::AccountPublicAddress& minerAddress, VarNote::Transaction& tx, uint64_t fee,
  VarNote::KeyPair* pTxKey = 0);
bool constructMinerTxBySize(const VarNote::Currency& currency, VarNote::Transaction& baseTransaction, uint32_t height,
  uint64_t alreadyGeneratedCoins, const VarNote::AccountPublicAddress& minerAddress,
  std::vector<size_t>& blockSizes, size_t targetTxSize, size_t targetBlockSize, uint64_t fee = 0,
  bool penalizeFee = false);
