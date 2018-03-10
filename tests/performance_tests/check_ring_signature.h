// Copyright (c) 2017-2018 YxomTech
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <vector>

#include "VarNote_core/account.h"
#include "VarNote_core/VarNote_basic.h"
#include "VarNote_core/VarNote_format_utils.h"
#include "crypto/crypto.h"

#include "multi_tx_test_base.h"

template<size_t a_ring_size>
class test_check_ring_signature : private multi_tx_test_base<a_ring_size>
{
  static_assert(0 < a_ring_size, "ring_size must be greater than 0");

public:
  static const size_t loop_count = a_ring_size < 100 ? 100 : 10;
  static const size_t ring_size = a_ring_size;

  typedef multi_tx_test_base<a_ring_size> base_class;

  bool init()
  {
    using namespace VarNote;

    if (!base_class::init())
      return false;

    m_alice.generate();

    std::vector<tx_destination_entry> destinations;
    destinations.push_back(tx_destination_entry(this->m_source_amount, m_alice.get_keys().m_account_address));

    if (!construct_tx(this->m_miners[this->real_source_idx].get_keys(), this->m_sources, destinations, std::vector<uint8_t>(), m_tx, 0))
      return false;

    get_transaction_prefix_hash(m_tx, m_tx_prefix_hash);

    return true;
  }

  bool test()
  {
    const VarNote::txin_to_key& txin = boost::get<VarNote::txin_to_key>(m_tx.vin[0]);
    return crypto::check_ring_signature(m_tx_prefix_hash, txin.k_image, this->m_public_key_ptrs, ring_size, m_tx.signatures[0].data());
  }

private:
  VarNote::account_base m_alice;
  VarNote::transaction m_tx;
  crypto::hash m_tx_prefix_hash;
};
