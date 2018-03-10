// Copyright (c) 2017-2018 YxomTech
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "VarNote_core/account.h"
#include "VarNote_core/VarNote_basic.h"
#include "VarNote_core/VarNote_format_utils.h"

#include "single_tx_test_base.h"

class test_is_out_to_acc : public single_tx_test_base
{
public:
  static const size_t loop_count = 1000;

  bool test()
  {
    const VarNote::txout_to_key& tx_out = boost::get<VarNote::txout_to_key>(m_tx.vout[0].target);
    return VarNote::is_out_to_acc(m_bob.get_keys(), tx_out, m_tx_pub_key, 0);
  }
};
