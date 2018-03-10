// Copyright (c) 2017-2018 YxomTech
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "IWallet.h"

#include <map>
#include <mutex>

namespace VarNote {

class WalletTxSendingState
{
public:
  enum State
  {
    SENDING,
    ERRORED,
    NOT_FOUND
  };

  void sending(TransactionId id);
  void sent(TransactionId id);
  void error(TransactionId id);
  State state(TransactionId id);

private:
  std::map<TransactionId, State> m_states;
};

} //namespace VarNote
