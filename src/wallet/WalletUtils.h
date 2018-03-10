// Copyright (c) 2017-2018 YxomTech
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <exception>
#include "WalletErrors.h"

namespace VarNote {

inline void throwIf(bool expr, VarNote::error::WalletErrorCodes ec)
{
  if (expr)
    throw std::system_error(make_error_code(ec));
}

} //namespace VarNote

