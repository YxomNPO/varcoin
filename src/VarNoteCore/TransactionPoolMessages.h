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

#include <VarNote.h>
#include <functional>

namespace VarNote {

enum class TransactionMessageType {
  AddTransactionType,
  DeleteTransactionType
};

// immutable messages
struct AddTransaction {
  Crypto::Hash hash;
};

struct DeleteTransaction {
  Crypto::Hash hash;
};

class TransactionPoolMessage {
public:
  TransactionPoolMessage(const AddTransaction& at);
  TransactionPoolMessage(const DeleteTransaction& at);

  // pattern matchin API
  void match(std::function<void(const AddTransaction&)>&&, std::function<void(const DeleteTransaction&)>&&);

  // API with explicit type handling
  TransactionMessageType getType() const;
  AddTransaction  getAddTransaction() const;
  DeleteTransaction getDeleteTransaction() const;

private:
  const TransactionMessageType type;
  union {
    const AddTransaction addTransaction;
    const DeleteTransaction deleteTransaction;
  };
};

TransactionPoolMessage makeAddTransaction(const Crypto::Hash& hash);
TransactionPoolMessage makeDelTransaction(const Crypto::Hash& hash);
}
