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

#include <unordered_map>
#include <map>

#include <IDataBase.h>
#include "VarNoteCore/BlockchainReadBatch.h"
#include "VarNoteCore/BlockchainWriteBatch.h"

namespace VarNote {

class DataBaseMock : public IDataBase {
public:
  DataBaseMock() = default;
  ~DataBaseMock() override;

  std::error_code write(IWriteBatch& batch) override;
  std::error_code writeSync(IWriteBatch& batch) override;
  std::error_code read(IReadBatch& batch) override;
  std::unordered_map<uint32_t, RawBlock> blocks();

  std::map<std::string, std::string> baseState;
};
}
