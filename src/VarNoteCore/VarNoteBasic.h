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

#include <boost/utility/value_init.hpp>
#include <VarNote.h>

namespace VarNote {
  const Crypto::Hash NULL_HASH = boost::value_initialized<Crypto::Hash>();
  const Crypto::PublicKey NULL_PUBLIC_KEY = boost::value_initialized<Crypto::PublicKey>();
  const Crypto::SecretKey NULL_SECRET_KEY = boost::value_initialized<Crypto::SecretKey>();

  KeyPair generateKeyPair();

  struct ParentBlockSerializer {
    ParentBlockSerializer(ParentBlock& parentBlock, uint64_t& timestamp, uint32_t& nonce, bool hashingSerialization, bool headerOnly) :
      m_parentBlock(parentBlock), m_timestamp(timestamp), m_nonce(nonce), m_hashingSerialization(hashingSerialization), m_headerOnly(headerOnly) {
    }

    ParentBlock& m_parentBlock;
    uint64_t& m_timestamp;
    uint32_t& m_nonce;
    bool m_hashingSerialization;
    bool m_headerOnly;
  };

  inline ParentBlockSerializer makeParentBlockSerializer(const BlockTemplate& b, bool hashingSerialization, bool headerOnly) {
    BlockTemplate& blockRef = const_cast<BlockTemplate&>(b);
    return ParentBlockSerializer(blockRef.parentBlock, blockRef.timestamp, blockRef.nonce, hashingSerialization, headerOnly);
  }

}
