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

#include <string>

namespace Tools
{
  class PasswordContainer
  {
  public:
    static const size_t max_password_size = 1024;

    PasswordContainer();
    PasswordContainer(std::string&& password);
    PasswordContainer(PasswordContainer&& rhs);
    ~PasswordContainer();

    void clear();
    bool empty() const { return m_empty; }
    const std::string& password() const { return m_password; }
    void password(std::string&& val) { m_password = std::move(val); m_empty = false; }
    bool read_password();

  private:
    bool read_from_file();
    bool read_from_tty();

  private:
    bool m_empty;
    std::string m_password;
  };
}
