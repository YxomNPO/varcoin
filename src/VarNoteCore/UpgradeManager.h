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

#include "IUpgradeManager.h"

#include <memory>

#include "IUpgradeDetector.h"

namespace VarNote {

//Simple upgrade manager version. It doesn't support voting for now.
class UpgradeManager: public IUpgradeManager {
public:
  UpgradeManager();
  virtual ~UpgradeManager();

  virtual void addMajorBlockVersion(uint8_t targetVersion, uint32_t upgradeHeight) override;

  virtual uint8_t getBlockMajorVersion(uint32_t blockIndex) const override;

private:
  std::vector<std::unique_ptr<IUpgradeDetector>> m_upgradeDetectors;
};

} //namespace VarNote
