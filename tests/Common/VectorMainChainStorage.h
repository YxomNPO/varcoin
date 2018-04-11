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

#include <VarNoteCore/IMainChainStorage.h>
#include <VarNoteCore/Currency.h>

#include <vector>

namespace VarNote {

class VectorMainChainStorage: public IMainChainStorage {
public:
  virtual void pushBlock(const RawBlock& rawBlock) override;
  virtual void popBlock() override;
  virtual RawBlock getBlockByIndex(uint32_t index) const override;
  virtual uint32_t getBlockCount() const override;
  virtual void clear() override;

private:
  std::vector<RawBlock> storage;
};

std::unique_ptr<IMainChainStorage> createVectorMainChainStorage(const Currency& currency);

}
