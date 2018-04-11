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

#include <cstdint>

#include "VarNoteProtocol/IVarNoteProtocolObserver.h"
#include "VarNoteProtocol/IVarNoteProtocolQuery.h"
#include "VarNoteProtocol/VarNoteProtocolHandlerCommon.h"

class IVarNoteProtocolQueryStub : public VarNote::IVarNoteProtocolHandler {
public:
  IVarNoteProtocolQueryStub() : peers(0), observedHeight(0), synchronized(false) {
  }

  virtual bool addObserver(VarNote::IVarNoteProtocolObserver* observer) override;
  virtual bool removeObserver(VarNote::IVarNoteProtocolObserver* observer) override;
  virtual uint32_t getObservedHeight() const override;
  virtual size_t getPeerCount() const override;
  virtual bool isSynchronized() const override;

  void setPeerCount(uint32_t count);
  void setObservedHeight(uint32_t height);

  void setSynchronizedStatus(bool status);
  virtual void relayBlock(VarNote::NOTIFY_NEW_BLOCK_request& arg) override{};
  virtual void relayTransactions(const std::vector<VarNote::BinaryArray>& transactions) override{};


private:
  size_t peers;
  uint32_t observedHeight;

  bool synchronized;
};
