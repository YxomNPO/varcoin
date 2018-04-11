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

#include <VarNoteCore/TransactionPoolMessages.h>

namespace VarNote {

TransactionPoolMessage::TransactionPoolMessage(const AddTransaction& at)
    : type(TransactionMessageType::AddTransactionType), addTransaction(at) {
}

TransactionPoolMessage::TransactionPoolMessage(const DeleteTransaction& dt)
    : type(TransactionMessageType::DeleteTransactionType), deleteTransaction(dt) {
}

// pattern match
void TransactionPoolMessage::match(std::function<void(const AddTransaction&)>&& addTxVisitor,
                                   std::function<void(const DeleteTransaction&)>&& delTxVisitor) {
  switch (getType()) {
    case TransactionMessageType::AddTransactionType:
      addTxVisitor(addTransaction);
      break;
    case TransactionMessageType::DeleteTransactionType:
      delTxVisitor(deleteTransaction);
      break;
  }
}

// API with explicit type handling
TransactionMessageType TransactionPoolMessage::getType() const {
  return type;
}

AddTransaction TransactionPoolMessage::getAddTransaction() const {
  assert(getType() == TransactionMessageType::AddTransactionType);
  return addTransaction;
}

DeleteTransaction TransactionPoolMessage::getDeleteTransaction() const {
  assert(getType() == TransactionMessageType::DeleteTransactionType);
  return deleteTransaction;
}

TransactionPoolMessage makeAddTransaction(const Crypto::Hash& hash) {
  return TransactionPoolMessage{AddTransaction{hash}};
}

TransactionPoolMessage makeDelTransaction(const Crypto::Hash& hash) {
  return TransactionPoolMessage{DeleteTransaction{hash}};
}
}
