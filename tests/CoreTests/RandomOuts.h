// Copyright (c) 2012-2018, The VarNote developers, YxomTech
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

#include "Chaingen.h"

namespace VarNote {

struct COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_response;

}

struct GetRandomOutputs : public test_chain_unit_base
{
  GetRandomOutputs();

  // bool check_tx_verification_context(VarNote::TransactionValidationError tve, bool tx_added, size_t event_idx, const VarNote::Transaction& tx);
  // bool check_block_verification_context(VarNote::BlockValidationError bve, size_t event_idx, const VarNote::BlockTemplate& block);
  // bool mark_last_valid_block(VarNote::Core& c, size_t ev_index, const std::vector<test_event_entry>& events);

  bool generate(std::vector<test_event_entry>& events) const;


private:

  bool checkHalfUnlocked(VarNote::Core& c, size_t ev_index, const std::vector<test_event_entry>& events);
  bool checkFullyUnlocked(VarNote::Core& c, size_t ev_index, const std::vector<test_event_entry>& events);

  bool request(VarNote::Core& c, uint64_t amount, size_t mixin, VarNote::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_response& resp);

};
