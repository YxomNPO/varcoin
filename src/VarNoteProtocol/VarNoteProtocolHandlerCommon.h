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

namespace VarNote
{
  struct NOTIFY_NEW_BLOCK_request;
  struct NOTIFY_NEW_TRANSACTIONS_request;

  /************************************************************************/
  /*                                                                      */
  /************************************************************************/
  struct i_varnote_protocol {
    virtual void relay_block(NOTIFY_NEW_BLOCK_request& arg) = 0;
    virtual void relay_transactions(NOTIFY_NEW_TRANSACTIONS_request& arg) = 0;
  };

  /************************************************************************/
  /*                                                                      */
  /************************************************************************/
  struct varnote_protocol_stub: public i_varnote_protocol {
    virtual void relay_block(NOTIFY_NEW_BLOCK_request& arg) override {}
    virtual void relay_transactions(NOTIFY_NEW_TRANSACTIONS_request& arg) override {}
  };
}
