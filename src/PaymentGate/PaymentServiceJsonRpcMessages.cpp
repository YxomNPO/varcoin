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

#include "PaymentServiceJsonRpcMessages.h"
#include "Serialization/SerializationOverloads.h"

namespace PaymentService {

void TransferDestination::serialize(VarNote::ISerializer& serializer) {
  bool r = serializer(amount, "amount");
  r &= serializer(address, "address");

  if (!r) {
    throw RequestSerializationError();
  }
}

void SendTransactionRequest::serialize(VarNote::ISerializer& serializer) {
  bool r = serializer(destinations, "destinations");
  r &= serializer(fee, "fee");
  r &= serializer(mixin, "mixin");
  serializer(unlockTime, "unlock_time");
  serializer(paymentId, "payment_id");

  if (!r) {
    throw RequestSerializationError();
  }
}

void SendTransactionResponse::serialize(VarNote::ISerializer& serializer) {
  serializer(transactionId, "transaction_id");
}

void GetAddressRequest::serialize(VarNote::ISerializer& serializer) {
  serializer(index, "index");
}

void DeleteAddressRequest::serialize(VarNote::ISerializer& serializer) {
  bool r = serializer(address, "address");

  if (!r) {
    throw RequestSerializationError();
  }
}

void DeleteAddressResponse::serialize(VarNote::ISerializer& serializer) {
}

void CreateAddressResponse::serialize(VarNote::ISerializer& serializer) {
  serializer(address, "address");
}

void GetAddressCountResponse::serialize(VarNote::ISerializer& serializer) {
  serializer(count, "count");
}

void GetAddressResponse::serialize(VarNote::ISerializer& serializer) {
  serializer(address, "address");
}

void GetActualBalanceRequest::serialize(VarNote::ISerializer& serializer) {
  bool r = serializer(address, "address");

  if (!r) {
    throw std::runtime_error("Required parameter is missing");
  }
}

void GetPendingBalanceRequest::serialize(VarNote::ISerializer& serializer) {
  serializer(address, "address");
}

void GetActualBalanceResponse::serialize(VarNote::ISerializer& serializer) {
  serializer(actualBalance, "actual_balance");
}

void GetPendingBalanceResponse::serialize(VarNote::ISerializer& serializer) {
  serializer(pendingBalance, "pending_balance");
}

void GetTransactionsCountResponse::serialize(VarNote::ISerializer& serializer) {
  serializer(transactionsCount, "transactions_count");
}

void GetTransfersCountResponse::serialize(VarNote::ISerializer& serializer) {
  serializer(transfersCount, "transfers_count");
}

void GetTransactionIdByTransferIdRequest::serialize(VarNote::ISerializer& serializer) {
  bool r = serializer(transferId, "transfer_id");

  if (!r) {
    throw RequestSerializationError();
  }
}

void GetTransactionIdByTransferIdResponse::serialize(VarNote::ISerializer& serializer) {
  serializer(transactionid, "transaction_id");
}

void GetTransactionRequest::serialize(VarNote::ISerializer& serializer) {
  bool r = serializer(transactionId, "transaction_id");

  if (!r) {
    throw RequestSerializationError();
  }
}

void TransactionRpcInfo::serialize(VarNote::ISerializer& serializer) {
  serializer(firstTransferId, "first_transfer_id");
  serializer(transferCount, "transfer_count");
  serializer(totalAmount, "total_amount");
  serializer(fee, "fee");
  serializer(hash, "hash");
  serializer(blockHeight, "block_height");
  serializer(timestamp, "timestamp");
  serializer(extra, "extra");
  serializer(transfers, "transfers");
}

void GetTransactionResponse::serialize(VarNote::ISerializer& serializer) {
  serializer(found, "found");

  if (found) {
    serializer(transactionInfo, "transaction_info");
  }
}

void ListTransactionsRequest::serialize(VarNote::ISerializer& serializer) {
  bool r = serializer(startingTransactionId, "starting_transaction_id");
  r &= serializer(maxTransactionCount, "max_transaction_count");

  if (!r) {
    throw std::runtime_error("Required parameter is missing");
  }
}

void ListTransactionsResponse::serialize(VarNote::ISerializer& serializer) {
  serializer(transactions, "transactions");
}

void TransferRpcInfo::serialize(VarNote::ISerializer& serializer) {
  serializer(address, "address");
  serializer(amount, "amount");
}

void GetTransferRequest::serialize(VarNote::ISerializer& serializer) {
  bool r = serializer(transferId, "transfer_id");

  if (!r) {
    throw RequestSerializationError();
  }
}

void GetTransferResponse::serialize(VarNote::ISerializer& serializer) {
  serializer(found, "found");
  if (found) {
    serializer(transferInfo, "transfer_info");
  }
}

void GetIncomingPaymentsRequest::serialize(VarNote::ISerializer& serializer) {
  bool r = serializer(payments, "payments");

  if (!r) {
    throw RequestSerializationError();
  }
}

void PaymentsById::serialize(VarNote::ISerializer& serializer) {
  serializer(id, "id");
  serializer(payments, "payments");
}

void GetIncomingPaymentsResponse::serialize(VarNote::ISerializer& serializer) {
  serializer(payments, "payments");
}

void PaymentDetails::serialize(VarNote::ISerializer& serializer) {
  serializer(txHash, "tx_hash");
  serializer(amount, "amount");
  serializer(blockHeight, "block_height");
  serializer(unlockTime, "unlock_time");
}

}
