// Copyright (c) 2017-2018 YxomTech
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once
#include "VarNote_core/difficulty.h"


namespace mining
{
  inline uint32_t get_target_for_difficulty(VarNote::difficulty_type difficulty)
  {
    if(!difficulty)
      return 0xffffffff;
    return 0xffffffff/static_cast<uint32_t>(difficulty);
  }

}
