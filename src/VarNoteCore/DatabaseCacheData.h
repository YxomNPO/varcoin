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

#include <VarNoteCore/BlockchainCache.h>
#include <map>

namespace VarNote {

struct KeyOutputInfo {
  Crypto::PublicKey publicKey;
  Crypto::Hash transactionHash;
  uint64_t unlockTime;
  uint16_t outputIndex;

  void serialize(VarNote::ISerializer& s);
};

// inherit here to avoid breaking IBlockchainCache interface
struct ExtendedTransactionInfo : CachedTransactionInfo {
  //CachedTransactionInfo tx;
  std::map<IBlockchainCache::Amount, std::vector<IBlockchainCache::GlobalOutputIndex>> amountToKeyIndexes; //global key output indexes spawned in this transaction
  std::map<IBlockchainCache::Amount, std::vector<IBlockchainCache::GlobalOutputIndex>> amountToMultiIndexes; //global multisignature output indexes spawned in this transaction
  void serialize(ISerializer& s);
};

}
