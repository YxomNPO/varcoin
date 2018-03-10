// Copyright (c) 2017-2018 YxomTech
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "VarNote_core/account.h"
#include "VarNote_core/VarNote_basic.h"
#include "VarNote_core/VarNote_format_utils.h"

#include "single_tx_test_base.h"

class test_generate_key_image_helper : public single_tx_test_base
{
public:
  static const size_t loop_count = 500;

  bool test()
  {
    VarNote::keypair in_ephemeral;
    crypto::key_image ki;
    return VarNote::generate_key_image_helper(m_bob.get_keys(), m_tx_pub_key, 0, in_ephemeral, ki);
  }
};
