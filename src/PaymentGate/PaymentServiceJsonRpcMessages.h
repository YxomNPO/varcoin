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

#include <exception>
#include <limits>
#include <vector>

#include "Serialization/ISerializer.h"

namespace PaymentService {

const uint32_t DEFAULT_ANONYMITY_LEVEL = 6;

class RequestSerializationError: public std::exception {
public:
  virtual const char* what() const throw() override { return "Request error"; }
};

struct Save {
  struct Request {
    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    void serialize(VarNote::ISerializer& serializer);
  };
};

struct Export {
  struct Request {
    std::string fileName;

    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    void serialize(VarNote::ISerializer& serializer);
  };
};

struct Reset {
  struct Request {
    std::string viewSecretKey;

    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    void serialize(VarNote::ISerializer& serializer);
  };
};

struct GetViewKey {
  struct Request {
    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    std::string viewSecretKey;

    void serialize(VarNote::ISerializer& serializer);
  };
};

struct GetStatus {
  struct Request {
    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    uint32_t blockCount;
    uint32_t knownBlockCount;
    std::string lastBlockHash;
    uint32_t peerCount;

    void serialize(VarNote::ISerializer& serializer);
  };
};

struct GetAddresses {
  struct Request {
    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    std::vector<std::string> addresses;

    void serialize(VarNote::ISerializer& serializer);
  };
};

struct CreateAddress {
  struct Request {
    std::string spendSecretKey;
    std::string spendPublicKey;

    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    std::string address;

    void serialize(VarNote::ISerializer& serializer);
  };
};

struct CreateAddressList {
  struct Request {
    std::vector<std::string> spendSecretKeys;

    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    std::vector<std::string> addresses;

    void serialize(VarNote::ISerializer& serializer);
  };
};

struct DeleteAddress {
  struct Request {
    std::string address;

    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    void serialize(VarNote::ISerializer& serializer);
  };
};

struct GetSpendKeys {
  struct Request {
    std::string address;

    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    std::string spendSecretKey;
    std::string spendPublicKey;

    void serialize(VarNote::ISerializer& serializer);
  };
};

struct GetBalance {
  struct Request {
    std::string address;

    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    uint64_t availableBalance;
    uint64_t lockedAmount;

    void serialize(VarNote::ISerializer& serializer);
  };
};

struct GetBlockHashes {
  struct Request {
    uint32_t firstBlockIndex;
    uint32_t blockCount;

    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    std::vector<std::string> blockHashes;

    void serialize(VarNote::ISerializer& serializer);
  };
};

struct TransactionHashesInBlockRpcInfo {
  std::string blockHash;
  std::vector<std::string> transactionHashes;

  void serialize(VarNote::ISerializer& serializer);
};

struct GetTransactionHashes {
  struct Request {
    std::vector<std::string> addresses;
    std::string blockHash;
    uint32_t firstBlockIndex = std::numeric_limits<uint32_t>::max();
    uint32_t blockCount;
    std::string paymentId;

    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    std::vector<TransactionHashesInBlockRpcInfo> items;

    void serialize(VarNote::ISerializer& serializer);
  };
};

struct TransferRpcInfo {
  uint8_t type;
  std::string address;
  int64_t amount;

  void serialize(VarNote::ISerializer& serializer);
};

struct TransactionRpcInfo {
  uint8_t state;
  std::string transactionHash;
  uint32_t blockIndex;
  uint64_t timestamp;
  bool isBase;
  uint64_t unlockTime;
  int64_t amount;
  uint64_t fee;
  std::vector<TransferRpcInfo> transfers;
  std::string extra;
  std::string paymentId;

  void serialize(VarNote::ISerializer& serializer);
};

struct GetTransaction {
  struct Request {
    std::string transactionHash;

    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    TransactionRpcInfo transaction;

    void serialize(VarNote::ISerializer& serializer);
  };
};

struct TransactionsInBlockRpcInfo {
  std::string blockHash;
  std::vector<TransactionRpcInfo> transactions;

  void serialize(VarNote::ISerializer& serializer);
};

struct GetTransactions {
  struct Request {
    std::vector<std::string> addresses;
    std::string blockHash;
    uint32_t firstBlockIndex = std::numeric_limits<uint32_t>::max();
    uint32_t blockCount;
    std::string paymentId;

    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    std::vector<TransactionsInBlockRpcInfo> items;

    void serialize(VarNote::ISerializer& serializer);
  };
};

struct GetUnconfirmedTransactionHashes {
  struct Request {
    std::vector<std::string> addresses;

    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    std::vector<std::string> transactionHashes;

    void serialize(VarNote::ISerializer& serializer);
  };
};

struct WalletRpcOrder {
  std::string address;
  uint64_t amount;

  void serialize(VarNote::ISerializer& serializer);
};

struct SendTransaction {
  struct Request {
    std::vector<std::string> sourceAddresses;
    std::vector<WalletRpcOrder> transfers;
    std::string changeAddress;
    uint64_t fee = 0;
    uint32_t anonymity = DEFAULT_ANONYMITY_LEVEL;
    std::string extra;
    std::string paymentId;
    uint64_t unlockTime = 0;

    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    std::string transactionHash;

    void serialize(VarNote::ISerializer& serializer);
  };
};

struct CreateDelayedTransaction {
  struct Request {
    std::vector<std::string> addresses;
    std::vector<WalletRpcOrder> transfers;
    std::string changeAddress;
    uint64_t fee = 0;
    uint32_t anonymity = DEFAULT_ANONYMITY_LEVEL;
    std::string extra;
    std::string paymentId;
    uint64_t unlockTime = 0;

    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    std::string transactionHash;

    void serialize(VarNote::ISerializer& serializer);
  };
};

struct GetDelayedTransactionHashes {
  struct Request {
    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    std::vector<std::string> transactionHashes;

    void serialize(VarNote::ISerializer& serializer);
  };
};

struct DeleteDelayedTransaction {
  struct Request {
    std::string transactionHash;

    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    void serialize(VarNote::ISerializer& serializer);
  };
};

struct SendDelayedTransaction {
  struct Request {
    std::string transactionHash;

    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    void serialize(VarNote::ISerializer& serializer);
  };
};

struct SendFusionTransaction {
  struct Request {
    uint64_t threshold;
    uint32_t anonymity = DEFAULT_ANONYMITY_LEVEL;
    std::vector<std::string> addresses;
    std::string destinationAddress;

    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    std::string transactionHash;

    void serialize(VarNote::ISerializer& serializer);
  };
};

struct EstimateFusion {
  struct Request {
    uint64_t threshold;
    std::vector<std::string> addresses;

    void serialize(VarNote::ISerializer& serializer);
  };

  struct Response {
    uint32_t fusionReadyCount;
    uint32_t totalOutputCount;

    void serialize(VarNote::ISerializer& serializer);
  };
};

} //namespace PaymentService
