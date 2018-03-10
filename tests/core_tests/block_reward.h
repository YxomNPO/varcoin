// Copyright (c) 2017-2018 YxomTech
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once 
#include "chaingen.h"

struct gen_block_reward : public test_chain_unit_base
{
  gen_block_reward();

  bool generate(std::vector<test_event_entry>& events) const;

  bool check_block_verification_context(const VarNote::block_verification_context& bvc, size_t event_idx, const VarNote::block& blk);

  bool mark_invalid_block(VarNote::core& c, size_t ev_index, const std::vector<test_event_entry>& events);
  bool mark_checked_block(VarNote::core& c, size_t ev_index, const std::vector<test_event_entry>& events);
  bool check_block_rewards(VarNote::core& c, size_t ev_index, const std::vector<test_event_entry>& events);

private:
  size_t m_invalid_block_index;
  std::vector<size_t> m_checked_blocks_indices;
};
