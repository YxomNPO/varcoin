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

#include "Serialization/ISerializer.h"

namespace VarNote {

struct CoreStatistics {
  uint64_t transactionPoolSize;
  uint64_t blockchainHeight;
  uint64_t miningSpeed;
  uint64_t alternativeBlockCount;
  std::string topBlockHashString;

  void serialize(ISerializer& s) {    
    s(transactionPoolSize, "tx_pool_size");
    s(blockchainHeight, "blockchain_height");
    s(miningSpeed, "mining_speed");
    s(alternativeBlockCount, "alternative_blocks");
    s(topBlockHashString, "top_block_id_str");
  }
};

}
