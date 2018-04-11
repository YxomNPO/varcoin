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
#include <map>
#include "VarNoteBasicImpl.h"
#include <Logging/LoggerRef.h>

namespace VarNote
{
  class Checkpoints
  {
  public:
    Checkpoints(Logging::ILogger& log);

    bool addCheckpoint(uint32_t index, const std::string& hash_str);
    bool isInCheckpointZone(uint32_t index) const;
    bool checkBlock(uint32_t index, const Crypto::Hash& h) const;
    bool checkBlock(uint32_t index, const Crypto::Hash& h, bool& isCheckpoint) const;
    bool isAlternativeBlockAllowed(uint32_t blockchainSize, uint32_t blockIndex) const;
    std::vector<uint32_t> getCheckpointHeights() const;

  private:
    std::map<uint32_t, Crypto::Hash> points;
    Logging::LoggerRef logger;
  };
}
