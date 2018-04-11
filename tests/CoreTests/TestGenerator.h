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

#include "VarNoteCore/Currency.h"
#include "TransactionBuilder.h"
#include <Logging/LoggerGroup.h>

class TestGenerator {
public:
  TestGenerator(
    const VarNote::Currency& currency, 
    std::vector<test_event_entry>& eventsRef) :
      generator(currency),
      events(eventsRef) {
    minerAccount.generate();
    //generator.constructBlock(genesisBlock, minerAccount, 1338224400);
    lastBlock = currency.genesisBlock();
    events.push_back(lastBlock);
  }

  const VarNote::Currency& currency() const { return generator.currency(); }

  void makeNextBlock(const std::list<VarNote::Transaction>& txs = std::list<VarNote::Transaction>()) {
   VarNote::BlockTemplate block;
    generator.constructBlock(block, lastBlock, minerAccount, txs);
    events.push_back(populateBlock(block, txs));
    lastBlock = block;
  }

  void makeNextBlock(const VarNote::Transaction& tx) {
    std::list<VarNote::Transaction> txs;
    txs.push_back(tx);
    makeNextBlock(txs);
  }

  void generateBlocks() {
    generateBlocks(currency().minedMoneyUnlockWindow());
  }

  void generateBlocks(size_t count, uint8_t majorVersion = VarNote::BLOCK_MAJOR_VERSION_1) {
    while (count--) {
     VarNote::BlockTemplate next;
      generator.constructBlockManually(next, lastBlock, minerAccount, test_generator::bf_major_ver, majorVersion);
      lastBlock = next;
      events.push_back(next);
    }
  }

  TransactionBuilder createTxBuilder(const VarNote::AccountBase& from, const VarNote::AccountBase& to, uint64_t amount, uint64_t fee) {

    std::vector<VarNote::TransactionSourceEntry> sources;
    std::vector<VarNote::TransactionDestinationEntry> destinations;

    fillTxSourcesAndDestinations(sources, destinations, from, to, amount, fee);

    TransactionBuilder builder(generator.currency());

    builder.setInput(sources, from.getAccountKeys());
    builder.setOutput(destinations);

    return builder;
  }

  void fillTxSourcesAndDestinations(
    std::vector<VarNote::TransactionSourceEntry>& sources, 
    std::vector<VarNote::TransactionDestinationEntry>& destinations,
    const VarNote::AccountBase& from, const VarNote::AccountBase& to, uint64_t amount, uint64_t fee, size_t nmix = 0) {
    fill_tx_sources_and_destinations(events, lastBlock, from, to, amount, fee, nmix, sources, destinations);
  }

  void constructTxToKey(
    VarNote::Transaction& tx,
    const VarNote::AccountBase& from,
    const VarNote::AccountBase& to,
    uint64_t amount,
    uint64_t fee,
    size_t nmix = 0) {
    construct_tx_to_key(logger, events, tx, lastBlock, from, to, amount, fee, nmix);
  }

  void addEvent(const test_event_entry& e) {
    events.push_back(e);
  }

  void addCallback(const std::string& name) {
    callback_entry cb;
    cb.callback_name = name;
    events.push_back(cb);
  }

  void addCheckAccepted() {
    addCallback("check_block_accepted");
  }

  void addCheckPurged() {
    addCallback("check_block_purged");
  }

  Logging::LoggerGroup logger;
  test_generator generator;
 VarNote::BlockTemplate genesisBlock;
 VarNote::BlockTemplate lastBlock;
  VarNote::AccountBase minerAccount;
  std::vector<test_event_entry>& events;
};
