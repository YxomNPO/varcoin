// Copyright (c) 2017-2018 YxomTech
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "p2p/net_node_common.h"
#include "VarNote_protocol/VarNote_protocol_defs.h"
#include "VarNote_core/connection_context.h"
namespace VarNote
{
  /************************************************************************/
  /*                                                                      */
  /************************************************************************/
  struct i_VarNote_protocol
  {
    virtual bool relay_block(NOTIFY_NEW_BLOCK::request& arg, VarNote_connection_context& exclude_context)=0;
    virtual bool relay_transactions(NOTIFY_NEW_TRANSACTIONS::request& arg, VarNote_connection_context& exclude_context)=0;
    //virtual bool request_objects(NOTIFY_REQUEST_GET_OBJECTS::request& arg, VarNote_connection_context& context)=0;
  };

  /************************************************************************/
  /*                                                                      */
  /************************************************************************/
  struct VarNote_protocol_stub: public i_VarNote_protocol
  {
    virtual bool relay_block(NOTIFY_NEW_BLOCK::request& arg, VarNote_connection_context& exclude_context)
    {
      return false;
    }
    virtual bool relay_transactions(NOTIFY_NEW_TRANSACTIONS::request& arg, VarNote_connection_context& exclude_context)
    {
      return false;
    }

  };
}
