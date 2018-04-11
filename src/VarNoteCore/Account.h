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

#include "VarNoteCore/VarNoteBasic.h"
#include "crypto/crypto.h"

namespace VarNote {

  class ISerializer;

  /************************************************************************/
  /*                                                                      */
  /************************************************************************/
  class AccountBase {
  public:
    AccountBase();
    void generate();

    const AccountKeys& getAccountKeys() const;
    void setAccountKeys(const AccountKeys& keys);
    uint64_t get_createtime() const { return m_creation_timestamp; }
    void set_createtime(uint64_t val) { m_creation_timestamp = val; }
    void serialize(ISerializer& s);

    template <class t_archive>
    inline void serialize(t_archive &a, const unsigned int /*ver*/) {
      a & m_keys;
      a & m_creation_timestamp;
    }

  private:
    void setNull();
    AccountKeys m_keys;
    uint64_t m_creation_timestamp;
  };
}
