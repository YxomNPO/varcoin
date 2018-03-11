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

#include "VarNoteCore/Account.h"
#include "VarNoteCore/VarNoteFormatUtils.h"
#include "VarNoteCore/Currency.h"

class TransactionBuilder {
public:

  typedef std::vector<VarNote::AccountKeys> KeysVector;
  typedef std::vector<Crypto::Signature> SignatureVector;
  typedef std::vector<SignatureVector> SignatureMultivector;

  struct MultisignatureSource {
    VarNote::MultisignatureInput input;
    KeysVector keys;
    Crypto::PublicKey srcTxPubKey;
    size_t srcOutputIndex;
  };

  TransactionBuilder(const VarNote::Currency& currency, uint64_t unlockTime = 0);

  // regenerate transaction keys
  TransactionBuilder& newTxKeys();
  TransactionBuilder& setTxKeys(const VarNote::KeyPair& txKeys);

  // inputs
  TransactionBuilder& setInput(const std::vector<VarNote::TransactionSourceEntry>& sources, const VarNote::AccountKeys& senderKeys);
  TransactionBuilder& addMultisignatureInput(const MultisignatureSource& source);

  // outputs
  TransactionBuilder& setOutput(const std::vector<VarNote::TransactionDestinationEntry>& destinations);
  TransactionBuilder& addOutput(const VarNote::TransactionDestinationEntry& dest);
  TransactionBuilder& addMultisignatureOut(uint64_t amount, const KeysVector& keys, uint32_t required);

  VarNote::Transaction build() const;

  std::vector<VarNote::TransactionSourceEntry> m_sources;
  std::vector<VarNote::TransactionDestinationEntry> m_destinations;

private:

  void fillInputs(VarNote::Transaction& tx, std::vector<VarNote::KeyPair>& contexts) const;
  void fillOutputs(VarNote::Transaction& tx) const;
  void signSources(const Crypto::Hash& prefixHash, const std::vector<VarNote::KeyPair>& contexts, VarNote::Transaction& tx) const;

  struct MultisignatureDestination {
    uint64_t amount;
    uint32_t requiredSignatures;
    KeysVector keys;
  };

  VarNote::AccountKeys m_senderKeys;

  std::vector<MultisignatureSource> m_msigSources;
  std::vector<MultisignatureDestination> m_msigDestinations;

  size_t m_version;
  uint64_t m_unlockTime;
  VarNote::KeyPair m_txKey;
  const VarNote::Currency& m_currency;
};
