// Copyright (c) 2017-2018 YxomTech
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "WalletUnconfirmedTransactions.h"
#include "VarNote_core/VarNote_format_utils.h"

namespace VarNote {

bool WalletUnconfirmedTransactions::findTransactionId(const crypto::hash& hash, TransactionId& id) {
  auto it = m_unconfirmedTxs.find(hash);

  if(it == m_unconfirmedTxs.end())
    return false;

  id = it->second.transactionId;

  return true;
}

void WalletUnconfirmedTransactions::erase(const crypto::hash& hash) {
  m_unconfirmedTxs.erase(hash);
}

void WalletUnconfirmedTransactions::add(const VarNote::transaction& tx,
    TransactionId transactionId, uint64_t change_amount) {
  UnconfirmedTransferDetails& utd = m_unconfirmedTxs[VarNote::get_transaction_hash(tx)];

  utd.change = change_amount;
  utd.sentTime = time(NULL);
  utd.tx = tx;
  utd.transactionId = transactionId;
}

uint64_t WalletUnconfirmedTransactions::countPendingBalance() const
{
  uint64_t amount = 0;

  for (auto& utx: m_unconfirmedTxs)
    amount+= utx.second.change;

  return amount;
}

} /* namespace VarNote */
