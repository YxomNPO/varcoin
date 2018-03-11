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

#include "VarNoteProtocol/IVarNoteProtocolObserver.h"
#include "VarNoteProtocol/IVarNoteProtocolQuery.h"

class IVarNoteProtocolQueryStub: public VarNote::IVarNoteProtocolQuery {
public:
  IVarNoteProtocolQueryStub() : peers(0), observedHeight(0), synchronized(false) {}

  virtual bool addObserver(VarNote::IVarNoteProtocolObserver* observer);
  virtual bool removeObserver(VarNote::IVarNoteProtocolObserver* observer);
  virtual uint32_t getObservedHeight() const;
  virtual size_t getPeerCount() const;
  virtual bool isSynchronized() const;

  void setPeerCount(uint32_t count);
  void setObservedHeight(uint32_t height);

  void setSynchronizedStatus(bool status);

private:
  size_t peers;
  uint32_t observedHeight;

  bool synchronized;
};
