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

#include "IVarNoteProtocolQueryStub.h"

bool IVarNoteProtocolQueryStub::addObserver(VarNote::IVarNoteProtocolObserver* observer) {
  return false;
}

bool IVarNoteProtocolQueryStub::removeObserver(VarNote::IVarNoteProtocolObserver* observer) {
  return false;
}

uint32_t IVarNoteProtocolQueryStub::getObservedHeight() const {
  return observedHeight;
}

size_t IVarNoteProtocolQueryStub::getPeerCount() const {
  return peers;
}

bool IVarNoteProtocolQueryStub::isSynchronized() const {
  return synchronized;
}

void IVarNoteProtocolQueryStub::setPeerCount(uint32_t count) {
  peers = count;
}

void IVarNoteProtocolQueryStub::setObservedHeight(uint32_t height) {
  observedHeight = height;
}

void IVarNoteProtocolQueryStub::setSynchronizedStatus(bool status) {
    synchronized = status;
}
