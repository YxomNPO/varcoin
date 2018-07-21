// Copyright (c) 2012-2018, The CryptoNote developers, YxomTech.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#include "BlockChain.hpp"

#include <boost/lexical_cast.hpp>
#include <chrono>
#include <iostream>
#include "VarNoteTools.hpp"
#include "DifficultyCheck.hpp"
#include "TransactionExtra.hpp"
#include "common/StringTools.hpp"
#include "common/Varint.hpp"
#include "rpc_api.hpp"
#include "seria/BinaryInputStream.hpp"
#include "seria/BinaryOutputStream.hpp"

using namespace varcoin;
using namespace platform;

static const std::string previous_versions[] = {"B"};  // most recent previous version should be first in list
                                                       // 1 -> 2 we fix difficulty for consensus
static const std::string version_1            = "1";
const std::string BlockChain::version_current = "3";
// We increment when making incompatible changes to indices.

// We use suffixes so all keys related to the same block are close to each other
// in DB
static const std::string BLOCK_PREFIX           = "b";
static const std::string BLOCK_SUFFIX           = "b";
static const std::string HEADER_PREFIX          = "b";
static const std::string HEADER_SUFFIX          = "h";
static const std::string TRANSATION_PREFIX      = "t";
static const size_t TRANSACTION_PREFIX_BYTES    = 5;  // We store only first bytes of tx hash in index
static const std::string TIP_CHAIN_PREFIX       = "c";
static const std::string TIMESTAMP_BLOCK_PREFIX = "T";

static const std::string CHILDREN_PREFIX = "x-ch/";
static const std::string CD_TIPS_PREFIX  = "x-tips/";
// We store bid->children counter, with counter=1 default (absent from index)
// We store cumulative_difficulty->bid for bids with no children

static const size_t HEADER_CACHE_MAX_SIZE          = 100000;  // when lots of read_header called without commit
static const std::string delete_blockchain_message = "database corrupted, please delete ";

bool Block::from_raw_block(const RawBlock &raw_block) {
	try {
		BlockTemplate &bheader = header;
		seria::from_binary(bheader, raw_block.block);
		transactions.resize(0);
		transactions.reserve(raw_block.transactions.size());
		for (auto &&raw_transaction : raw_block.transactions) {
			Transaction transaction;
			seria::from_binary(transaction, raw_transaction);
			transactions.push_back(std::move(transaction));
		}
	} catch (...) {
		return false;
	}
	return true;
}

bool Block::to_raw_block(RawBlock &raw_block) const {
	try {
		const BlockTemplate &bheader = header;
		raw_block.block              = seria::to_binary(bheader);
		raw_block.transactions.resize(0);
		raw_block.transactions.reserve(transactions.size());
		for (auto &&transaction : transactions) {
			BinaryArray raw_transaction = seria::to_binary(transaction);
			raw_block.transactions.push_back(std::move(raw_transaction));
		}
	} catch (...) {
		return false;
	}
	return true;
}

PreparedBlock::PreparedBlock(BinaryArray &&ba, crypto::CryptoNightContext *context) : block_data(std::move(ba)) {
	seria::from_binary(raw_block, block_data);
	if (block.from_raw_block(raw_block))
		bid = varcoin::get_block_hash(block.header);
	if (block.header.major_version >= 2)
		parent_block_size = seria::binary_size(block.header.parent_block);
	coinbase_tx_size      = seria::binary_size(block.header.base_transaction);
	base_transaction_hash = get_transaction_hash(block.header.base_transaction);
	if (context)
		long_block_hash = varcoin::get_block_long_hash(block.header, *context);
}

PreparedBlock::PreparedBlock(RawBlock &&rba, crypto::CryptoNightContext *context) : raw_block(rba) {
	block_data = seria::to_binary(raw_block);
	if (block.from_raw_block(raw_block))
		bid = varcoin::get_block_hash(block.header);
	if (block.header.major_version >= 2)
		parent_block_size = seria::binary_size(block.header.parent_block);
	coinbase_tx_size      = seria::binary_size(block.header.base_transaction);
	base_transaction_hash = get_transaction_hash(block.header.base_transaction);
	if (context)
		long_block_hash = varcoin::get_block_long_hash(block.header, *context);
}

BlockChain::BlockChain(const Hash &genesis_bid, const std::string &coin_folder, bool read_only)
    : m_genesis_bid(genesis_bid), m_coin_folder(coin_folder), m_db(read_only, coin_folder + "/blockchain") {
	std::string version;
	if (!m_db.get("$version", version)) {
		DB::Cursor cur = m_db.begin(std::string());
		if (!cur.end())
			throw std::runtime_error(
			    "Blockchain database format unknown version, please delete " + coin_folder + "/blockchain");
		version = version_current;
		m_db.put("$version", version, false);
	}
	if (version == previous_versions[0]) {
		std::cout << "Data format changed, old version=" << version << " current version=" << version_current
		          << ", deleting varcoind cache..." << std::endl;
		std::set<Hash> main_chain_bids;
		for (Height ha = 1;; ha += 1) {
			BinaryArray ba;
			if (!m_db.get(TIP_CHAIN_PREFIX + previous_versions[0] + "/" + common::write_varint_sqlite4(ha), ba))
				break;
			Hash bid;
			seria::from_binary(bid, ba);
			main_chain_bids.insert(bid);
		}
		std::cout << "Found " << main_chain_bids.size() << " blocks from main chain" << std::endl;
		size_t erased = 0, skipped = 0;
		size_t total_items = m_db.get_approximate_items_count();
		for (DB::Cursor cur = m_db.rbegin(std::string()); !cur.end();) {
			if ((erased + skipped) % 1000000 == 0)
				std::cout << "Processed " << (erased + skipped) / 1000000 << "/" << (total_items + 999999) / 1000000
				          << " million DB records" << std::endl;
			if (cur.get_suffix().find(TIP_CHAIN_PREFIX + previous_versions[0] + "/") == 0) {
				cur.next();
				skipped += 1;
				continue;  // chain
			}
			if (cur.get_suffix().find(BLOCK_PREFIX) == 0 &&
			    cur.get_suffix().substr(cur.get_suffix().size() - BLOCK_SUFFIX.size()) == BLOCK_SUFFIX) {
				Hash bid;
				DB::from_binary_key(cur.get_suffix(), BLOCK_PREFIX.size(), bid.data, sizeof(bid.data));
				if (main_chain_bids.count(bid) != 0) {
					cur.next();
					skipped += 1;
					continue;  // block in main chain
				}
			}
			cur.erase();
			erased += 1;
		}
		version = "3";  // Deleting all block headers fixes difficulty and checks fast consensus
		m_db.put("$version", version, false);
		std::cout << "Deleted " << erased << " records, skipped " << skipped << " records" << std::endl;
		db_commit();
	}
	Hash stored_genesis_bid;
	if (read_chain(0, &stored_genesis_bid)) {
		if (stored_genesis_bid != genesis_bid)
			throw std::runtime_error("Database starts with different genesis_block");
		read_tip();
	}
	DB::Cursor cur2 = m_db.rbegin(TIP_CHAIN_PREFIX + previous_versions[0] + "/");
	m_internal_import_known_height =
	    cur2.end() ? 0 : boost::lexical_cast<Height>(common::read_varint_sqlite4(cur2.get_suffix()));
	//	test_print_structure();
}

void BlockChain::db_commit() {
	std::cout << "BlockChain::db_commit started... tip_height=" << m_tip_height
	          << " header_cache.size=" << header_cache.size() << std::endl;
	m_db.commit_db_txn();
	header_cache.clear();  // Most simple cache policy ever
	std::cout << "BlockChain::db_commit finished..." << std::endl;
}

BroadcastAction BlockChain::add_block(const PreparedBlock &pb, api::BlockHeader *info) {
	*info             = api::BlockHeader();
	bool have_header = read_header(pb.bid, info);
	bool have_block  = has_block(pb.bid);
	if (have_block && have_header)
		return BroadcastAction::NOTHING;
	api::BlockHeader prev_info;
	prev_info.height = -1;
	if (pb.bid != m_genesis_bid && !read_header(pb.block.header.previous_block_hash, &prev_info))
		return BroadcastAction::NOTHING;  // Not interested in orphan headers
	//	std::cout << "pb.ph=" << common::pod_to_hex(pb.block.header.parent_block.previous_block_hash) << std::endl;
	info->major_version       = pb.block.header.major_version;
	info->minor_version       = pb.block.header.minor_version;
	info->timestamp           = pb.block.header.timestamp;
	info->previous_block_hash = pb.block.header.previous_block_hash;
	info->nonce               = pb.block.header.nonce;
	info->hash                = pb.bid;
	info->height              = prev_info.height + 1;
	// Rest fields are filled by check_standalone_consensus
	std::string check_error = check_standalone_consensus(pb, info, prev_info, true);
	Hash first_difficulty_check_hash;
	if (!common::pod_from_hex(difficulty_check[0].hash, first_difficulty_check_hash))
		throw std::logic_error("DifficultyCheck table corrupted");
	if (info->hash == first_difficulty_check_hash &&
	    info->cumulative_difficulty != difficulty_check[0].cumulative_difficulty) {
		std::cout << "Reached first difficulty checkpoint with wrong cumulative_difficulty "
		          << info->cumulative_difficulty << ", should be " << difficulty_check[0].cumulative_difficulty << ", "
		          << delete_blockchain_message << m_coin_folder << "/blockchain" << std::endl;
		std::exit(api::VARCOIND_DATABASE_ERROR);
	}
	if (!check_error.empty())
		return BroadcastAction::BAN;  // TODO - return check_error
	try {
		if (!have_block)
			store_block(pb.bid, pb.block_data);  // Do not commit between here and
		                                         // reorganize_blocks or invariant
		                                         // might be dead
		store_header(pb.bid, *info);
		if (pb.bid == m_genesis_bid) {
			if (!redo_block(pb.bid, pb.raw_block, pb.block, *info, pb.base_transaction_hash))
				throw std::logic_error("Failed to apply genesis block");
			push_chain(pb.bid, info->cumulative_difficulty);
		} else {
			modify_children_counter(prev_info.cumulative_difficulty, pb.block.header.previous_block_hash, 1);
		}
		check_children_counter(info->cumulative_difficulty, pb.bid, 1);
		modify_children_counter(info->cumulative_difficulty, pb.bid, -1);  // -1 from default 1 gives 0
		if (info->cumulative_difficulty > m_tip_cumulative_difficulty) {
			if (get_tip_bid() == pb.block.header.previous_block_hash) {  // most common case optimization
				if (!redo_block(pb.bid, pb.raw_block, pb.block, *info, pb.base_transaction_hash))
					return BroadcastAction::BAN;
				push_chain(pb.bid, info->cumulative_difficulty);
				//				std::map<Hash, std::pair<Transaction, BinaryArray>> undone_transactions;
				//				on_reorganization(undone_transactions, true);
			} else
				reorganize_blocks(pb.bid, pb, *info);
		}
	} catch (const std::exception &ex) {
		std::cout << "Exception while reorganizing blockchain, probably out of "
		             "disk space ex.what="
		          << ex.what() << ", " << delete_blockchain_message << m_coin_folder << "/blockchain" << std::endl;
		std::exit(api::VARCOIND_DATABASE_ERROR);
	}
	if (get_tip_height() % 50000 == 0)
		db_commit();
	return BroadcastAction::BROADCAST_ALL;
}

bool BlockChain::reorganize_blocks(const Hash &switch_to_chain,
    const PreparedBlock &recent_pb,
    const api::BlockHeader &recent_info) {
	// Header chain is better than block chain, undo upto splitting block
	std::vector<Hash> chain1, chain2;
	Hash common = get_common_block(m_tip_bid, switch_to_chain, &chain1, &chain2);
	for (auto &&chha : chain2) {
		if (!has_block(chha))
			return false;  // Full new chain not yet downloaded
	}
	std::map<Hash, std::pair<Transaction, BinaryArray>> undone_transactions;
	bool undone_blocks = false;
	while (m_tip_bid != common) {
		RawBlock raw_block;
		Block block;
		if (!read_block(m_tip_bid, &raw_block) || !block.from_raw_block(raw_block))
			throw std::logic_error("Block to undo not found or failed to convert" + common::pod_to_hex(m_tip_bid));
		undone_blocks = true;
		undo_block(m_tip_bid, raw_block, block, m_tip_height);
		for (size_t tx_index = 0; tx_index != block.transactions.size(); ++tx_index) {
			Hash tid = block.header.transaction_hashes.at(tx_index);
			undone_transactions.insert(std::make_pair(tid, std::make_pair(std::move(block.transactions.at(tx_index)),
			                                                   std::move(raw_block.transactions.at(tx_index)))));
		}
		pop_chain();
		m_tip_bid                   = block.header.previous_block_hash;
		api::BlockHeader info       = get_tip();
		m_tip_cumulative_difficulty = info.cumulative_difficulty;
		if (read_chain(m_tip_height) != m_tip_bid)
			throw std::logic_error(
			    "Invariant dead - after undo tip does not match read_chain" + common::pod_to_hex(m_tip_bid));
		tip_changed();
	}
	// Now redo all blocks we have in storage, will ask for the rest of blocks
	bool result = true;
	while (!chain2.empty()) {
		Hash chha = chain2.back();
		chain2.pop_back();
		if (chha == recent_pb.bid) {
			if (recent_pb.block.header.previous_block_hash != m_tip_bid)
				throw std::logic_error("Unexpected block prev, invariant dead");
			if (!redo_block(recent_pb.bid, recent_pb.raw_block, recent_pb.block, recent_info,
			        recent_pb.base_transaction_hash)) {
				// invalid block on longest subchain, make no attempt to download the
				// rest
				// we will forever stuck on this block until longer chain appears, that
				// does not include it
				result = false;
				break;
			}
			push_chain(chha, recent_info.cumulative_difficulty);
			for (auto &&tid : recent_pb.block.header.transaction_hashes)
				undone_transactions.erase(tid);
		} else {
			RawBlock raw_block;
			Block block;
			if (!read_block(chha, &raw_block) || !block.from_raw_block(raw_block)) {
				result = false;
				break;  // Strange, we checked has_block, somehow "bad block" got
				        // into DB. TODO - throw?
			}
			if (block.header.previous_block_hash != m_tip_bid)
				throw std::logic_error("Unexpected block prev, invariant dead");
			api::BlockHeader info = read_header(chha);
			// if redo fails, we will forever stuck on this block until longer chain
			// appears, that does not include it
			if (!redo_block(chha, raw_block, block, info, get_transaction_hash(block.header.base_transaction))) {
				result = false;
				break;
			}
			push_chain(chha, info.cumulative_difficulty);
			for (auto &&tid : block.header.transaction_hashes)
				undone_transactions.erase(tid);
		}
	}
	on_reorganization(undone_transactions, undone_blocks);
	return result;
}

Hash BlockChain::get_common_block(
    const Hash &bid1, const Hash &bid2, std::vector<Hash> *chain1, std::vector<Hash> *chain2) const {
	Hash hid1            = bid1;
	Hash hid2            = bid2;
	api::BlockHeader ha1 = read_header(hid1);
	api::BlockHeader ha2 = read_header(hid2);
	if (chain1)
		chain1->clear();
	if (chain2)
		chain2->clear();
	while (ha1.height > ha2.height) {
		if (chain1)
			chain1->push_back(hid1);
		hid1 = ha1.previous_block_hash;
		ha1  = read_header(hid1);
	}
	while (ha2.height > ha1.height) {
		if (chain2)
			chain2->push_back(hid2);
		hid2 = ha2.previous_block_hash;
		ha2  = read_header(hid2);
	}
	while (hid1 != hid2) {
		if (chain1)
			chain1->push_back(hid1);
		hid1 = ha1.previous_block_hash;
		ha1  = read_header(hid1);

		if (chain2)
			chain2->push_back(hid2);
		hid2 = ha2.previous_block_hash;
		ha2  = read_header(hid2);
	}
	return hid1;
}

std::vector<Hash> BlockChain::get_sparse_chain() const {
	std::vector<Hash> tip_path;

	uint32_t jump = 0;
	while (m_tip_height >= jump) {
		tip_path.push_back(read_chain(m_tip_height - jump));
		if (tip_path.size() <= 10)
			jump += 1;
		else
			jump += (1 << (tip_path.size() - 10));
	}
	if (tip_path.back() != m_genesis_bid)
		tip_path.push_back(m_genesis_bid);
	return tip_path;
}

std::vector<api::BlockHeader> BlockChain::get_sync_headers(const std::vector<Hash> &locator, size_t max_count) const {
	std::vector<api::BlockHeader> result;
	Height start_height     = 0;
	std::vector<Hash> chain = get_sync_headers_chain(locator, &start_height, max_count);
	result.reserve(chain.size());
	for (auto &&c : chain) {
		result.push_back(read_header(c));
	}
	return result;
}

uint32_t BlockChain::find_blockchain_supplement(const std::vector<Hash> &remote_block_ids) const {
	for (auto &&lit : remote_block_ids) {
		api::BlockHeader header;
		if (!read_header(lit, &header))
			continue;
		if (header.height > m_tip_height)
			continue;
		return header.height;
	}
	return 0;  // Not possible if genesis blocks match
}

Height BlockChain::get_timestamp_lower_bound_block_index(Timestamp ts) const {
	auto middle    = common::write_varint_sqlite4(ts);
	DB::Cursor cur = m_db.begin(TIMESTAMP_BLOCK_PREFIX, middle);
	if (cur.end())
		return m_tip_height;
	const char *be = cur.get_suffix().data();
	const char *en = be + cur.get_suffix().size();
	common::read_varint_sqlite4(be, en);  // We ignore result, auto actual_ts =
	return boost::lexical_cast<Height>(common::read_varint_sqlite4(be, en));
}

std::vector<Hash> BlockChain::get_sync_headers_chain(const std::vector<Hash> &locator,
    Height *start_height,
    size_t max_count) const {
	std::vector<Hash> result;
	for (auto &&lit : locator) {
		api::BlockHeader header;
		if (!read_header(lit, &header))
			continue;
		if (header.height > m_tip_height) {  // Asker has better chain then we do
			*start_height = m_tip_height + 1;
			return result;
		}
		uint32_t min_height = header.height;
		Hash loc_ha         = lit;
		for (; min_height != 0; min_height -= 1) {
			Hash ha = read_chain(min_height);
			if (ha == loc_ha)
				break;
			loc_ha = header.previous_block_hash;
			header = read_header(loc_ha);
		}
		*start_height = min_height;
		for (; result.size() < max_count && min_height <= m_tip_height; min_height += 1) {
			result.push_back(read_chain(min_height));
		}
		return result;
	}
	*start_height = m_tip_height + 1;
	return result;
}

struct APIRawBlockHeightDifficulty {
	RawBlock &raw_block;
	Height &height;
	Difficulty &cd;
	APIRawBlockHeightDifficulty(RawBlock &raw_block, Height &height, Difficulty &cd)
	    : raw_block(raw_block), height(height), cd(cd) {}
};

namespace seria {
void ser_members(APIRawBlockHeightDifficulty &v, ISeria &s) {
	seria_kv("height", v.height, s);
	seria_kv("cd", v.cd, s);
	seria_kv("raw_block", v.raw_block, s);
}
}  // namespace seria

struct APITransactionPos {
	Hash bid{};
	uint32_t index = 0;
	size_t pos     = 0;
	size_t size    = 0;
};

namespace seria {
void ser_members(APITransactionPos &v, ISeria &s) {
	seria_kv("bid", v.bid, s);
	seria_kv("index", v.index, s);
	seria_kv("pos", v.pos, s);
	seria_kv("size", v.size, s);
}
}  // namespace seria

bool BlockChain::read_transaction(
    const Hash &tid, Transaction *tx, Height *block_height, Hash *block_hash, size_t *index_in_block) const {
	auto txkey = TRANSATION_PREFIX + DB::to_binary_key(tid.data, TRANSACTION_PREFIX_BYTES);
	for (DB::Cursor cur = m_db.begin(txkey); !cur.end(); cur.next()) {
		const std::string &suf = cur.get_suffix();
		const char *be         = suf.data();
		const char *en         = be + suf.size();
		Height ha              = boost::lexical_cast<Height>(common::read_varint_sqlite4(be, en));
		size_t in              = boost::lexical_cast<size_t>(common::read_varint_sqlite4(be, en));
		Hash bid;
		if (!read_chain(ha, &bid))
			throw std::logic_error("transaction index corrupted while reading tid=" + common::pod_to_hex(tid));
		RawBlock rb;
		Block block;
		if (!read_block(bid, &rb) || !block.from_raw_block(rb))
			throw std::logic_error("transaction index corrupted while reading bid=" + common::pod_to_hex(bid));
		if (in == 0) {
			if (get_transaction_hash(block.header.base_transaction) != tid)
				continue;
			*tx = block.header.base_transaction;
		} else {
			if (block.header.transaction_hashes.at(in - 1) != tid)
				continue;
			*tx = block.transactions.at(in - 1);
		}
		*block_hash     = bid;
		*block_height   = ha;
		*index_in_block = in;
		return true;
	}
	return false;
}

bool BlockChain::redo_block(const Hash &bhash, const RawBlock &, const Block &block, const api::BlockHeader &info,
    const Hash &base_transaction_hash) {
	if (!redo_block(bhash, block, info))
		return false;
	auto tikey = TIMESTAMP_BLOCK_PREFIX + common::write_varint_sqlite4(info.timestamp) +
	             common::write_varint_sqlite4(info.height);
	m_db.put(tikey, std::string(), true);

	auto bkey = TRANSATION_PREFIX + DB::to_binary_key(base_transaction_hash.data, TRANSACTION_PREFIX_BYTES) +
	            common::write_varint_sqlite4(info.height) + common::write_varint_sqlite4(0);
	m_db.put(bkey, std::string(), true);
	for (size_t tx_index = 0; tx_index != block.transactions.size(); ++tx_index) {
		Hash tid = block.header.transaction_hashes.at(tx_index);
		bkey     = TRANSATION_PREFIX + DB::to_binary_key(tid.data, TRANSACTION_PREFIX_BYTES) +
		       common::write_varint_sqlite4(info.height) + common::write_varint_sqlite4(tx_index + 1);
		m_db.put(bkey, std::string(), true);
	}

	//	m_tip_segment.push_back(info);
	//	if (m_tip_segment.size() > 2048)  // TODO - should be enough for all block windows we use
	//		m_tip_segment.pop_front();
	return true;
}
void BlockChain::undo_block(const Hash &bhash, const RawBlock &, const Block &block, Height height) {
	//	if (!m_tip_segment.empty())
	//		m_tip_segment.pop_back();
	undo_block(bhash, block, height);

	auto tikey = TIMESTAMP_BLOCK_PREFIX + common::write_varint_sqlite4(block.header.timestamp) +
	             common::write_varint_sqlite4(height);
	m_db.del(tikey, true);

	Hash tid  = get_transaction_hash(block.header.base_transaction);
	auto bkey = TRANSATION_PREFIX + DB::to_binary_key(tid.data, TRANSACTION_PREFIX_BYTES) +
	            common::write_varint_sqlite4(height) + common::write_varint_sqlite4(0);
	m_db.del(bkey, true);
	for (size_t tx_index = 0; tx_index != block.transactions.size(); ++tx_index) {
		tid  = block.header.transaction_hashes.at(tx_index);
		bkey = TRANSATION_PREFIX + DB::to_binary_key(tid.data, TRANSACTION_PREFIX_BYTES) +
		       common::write_varint_sqlite4(height) + common::write_varint_sqlite4(tx_index + 1);
		m_db.del(bkey, true);
	}
}

void BlockChain::store_block(const Hash &bid, const BinaryArray &block_data) {
	auto key = BLOCK_PREFIX + DB::to_binary_key(bid.data, sizeof(bid.data)) + BLOCK_SUFFIX;
	m_db.put(key, block_data, true);
}

bool BlockChain::read_block(const Hash &bid, RawBlock *raw_block) const {
	BinaryArray rb;
	auto key = BLOCK_PREFIX + DB::to_binary_key(bid.data, sizeof(bid.data)) + BLOCK_SUFFIX;
	if (!m_db.get(key, rb))
		return false;
	seria::from_binary(*raw_block, rb);
	return true;
}

bool BlockChain::has_block(const Hash &bid) const {
	platform::DB::Value ms;
	auto key = BLOCK_PREFIX + DB::to_binary_key(bid.data, sizeof(bid.data)) + BLOCK_SUFFIX;
	if (!m_db.get(key, ms))
		return false;
	return true;
}

void BlockChain::store_header(const Hash &bid, const api::BlockHeader &header) {
	auto key       = HEADER_PREFIX + DB::to_binary_key(bid.data, sizeof(bid.data)) + HEADER_SUFFIX;
	BinaryArray ba = seria::to_binary(header);
	m_db.put(key, ba, true);
}

bool BlockChain::read_header(const Hash &bid, api::BlockHeader *header) const {
	auto cit = header_cache.find(bid);
	if (cit != header_cache.end()) {
		*header = cit->second;
		return true;
	}
	if (header_cache.size() > HEADER_CACHE_MAX_SIZE) {
		std::cout << "HEADER_CACHE_MAX_SIZE cleared" << std::endl;
		header_cache.clear();  // very simple policy
	}
	BinaryArray rb;
	auto key = HEADER_PREFIX + DB::to_binary_key(bid.data, sizeof(bid.data)) + HEADER_SUFFIX;
	if (!m_db.get(key, rb))
		return false;
	Hash bbid = bid;  // next line can modify bid, because it can be reference to header.previous_block_hash
	seria::from_binary(*header, rb);
	header_cache.insert(std::make_pair(bbid, *header));
	return true;
}

api::BlockHeader BlockChain::read_header(const Hash &bid) const {
	api::BlockHeader result;
	if (!read_header(bid, &result))
		throw std::logic_error("Expected header was not found" + common::pod_to_hex(bid));
	return result;
}

const api::BlockHeader &BlockChain::get_tip() const {
	auto cit = header_cache.find(get_tip_bid());
	if (cit != header_cache.end()) {
		return cit->second;
	}
	read_header(get_tip_bid());
	cit = header_cache.find(get_tip_bid());
	if (cit == header_cache.end())
		throw std::logic_error("After read_header, header should be in header_cache");
	return cit->second;
	//	if (m_tip_segment.empty())
	//		m_tip_segment.push_back(read_header(get_tip_bid()));
	//	return m_tip_segment.back();
}

std::vector<api::BlockHeader> BlockChain::get_tip_segment(
    const api::BlockHeader &prev_info, Height window, bool add_genesis) const {
	std::vector<api::BlockHeader> result;
	if (prev_info.height == Height(-1))
		return result;
	api::BlockHeader pi = prev_info;
	while (result.size() < window && pi.height != 0) {
		result.push_back(pi);
		if (!read_header(pi.previous_block_hash, &pi))
			throw std::logic_error("Invariant dead - previous block header not found in get_tip_segment");
	}
	if (result.size() < window && add_genesis) {
		if (pi.height != 0)
			throw std::logic_error(
			    "Invariant dead - window size not reached, but genesis not found in get_tip_segment");
		result.push_back(pi);
	}
	//	if (get_tip_height() == (Height)-1 || height_delta > get_tip_height())
	//		return std::make_pair(m_tip_segment.end(), m_tip_segment.end());
	//	while (m_tip_segment.size() < height_delta + window && m_tip_segment.size() < m_tip_height + 1) {
	//		Hash ha = read_chain(static_cast<uint32_t>(m_tip_height - m_tip_segment.size()));
	//		m_tip_segment.push_front(read_header(ha));
	//	}
	//	if (m_tip_height + 1 <= height_delta + window) {
	//			if( m_tip_segment.size() == m_tip_height + 1 ) {
	//	if (height_delta + window >= m_tip_segment.size()) {
	//		return std::make_pair(m_tip_segment.begin() + (add_genesis ? 0 : 1), m_tip_segment.end() - height_delta);
	//	}
	std::reverse(result.begin(), result.end());
	return result;  // std::make_pair(m_tip_segment.end() - window - height_delta, m_tip_segment.end() - height_delta);
}

void BlockChain::read_tip() {
	DB::Cursor cur2 = m_db.rbegin(TIP_CHAIN_PREFIX + version_1 + "/");
	m_tip_height    = cur2.end() ? -1 : boost::lexical_cast<Height>(common::read_varint_sqlite4(cur2.get_suffix()));
	seria::from_binary(m_tip_bid, cur2.get_value_array());
	api::BlockHeader tip_block  = read_header(m_tip_bid);
	m_tip_cumulative_difficulty = tip_block.cumulative_difficulty;
}

void BlockChain::push_chain(Hash bid, Difficulty cumulative_difficulty) {
	m_tip_height += 1;
	BinaryArray ba = seria::to_binary(bid);
	m_db.put(TIP_CHAIN_PREFIX + version_1 + "/" + common::write_varint_sqlite4(m_tip_height), ba, true);
	m_tip_bid                   = bid;
	m_tip_cumulative_difficulty = cumulative_difficulty;
	tip_changed();
}

void BlockChain::pop_chain() {
	if (m_tip_height == 0)
		throw std::logic_error("pop_chain tip_height == 0");
	m_db.del(TIP_CHAIN_PREFIX + version_1 + "/" + common::write_varint_sqlite4(m_tip_height), true);
	m_tip_height -= 1;
}

bool BlockChain::read_chain(uint32_t height, Hash *bid) const {
	BinaryArray ba;
	if (!m_db.get(TIP_CHAIN_PREFIX + version_1 + "/" + common::write_varint_sqlite4(height), ba))
		return false;
	seria::from_binary(*bid, ba);
	return true;
}

Hash BlockChain::read_chain(uint32_t height) const {
	Hash ha;
	if (!read_chain(height, &ha))
		throw std::logic_error("read_header_chain failed");
	return ha;
}

void BlockChain::check_children_counter(Difficulty cd, const Hash &bid, int value) {
	auto key    = CHILDREN_PREFIX + DB::to_binary_key(bid.data, sizeof(bid.data));
	auto cd_key = CD_TIPS_PREFIX + common::write_varint_sqlite4(cd) + DB::to_binary_key(bid.data, sizeof(bid.data));
	int counter = 1;  // default is 1 when not stored in db
	BinaryArray rb;
	if (m_db.get(key, rb))
		seria::from_binary(counter, rb);
	if (counter != value)
		throw std::logic_error("check_children_counter index corrupted");
	if (counter == 0 && !m_db.get(cd_key, rb))
		throw std::logic_error("check_children_counter tip is not in index");
	if (counter != 0 && m_db.get(cd_key, rb))
		throw std::logic_error("check_children_counter non-tip is in index");
}

void BlockChain::modify_children_counter(Difficulty cd, const Hash &bid, int delta) {
	auto key    = CHILDREN_PREFIX + DB::to_binary_key(bid.data, sizeof(bid.data));
	auto cd_key = CD_TIPS_PREFIX + common::write_varint_sqlite4(cd) + DB::to_binary_key(bid.data, sizeof(bid.data));
	uint32_t counter = 1;  // default is 1 when not stored in db
	BinaryArray rb;
	if (m_db.get(key, rb))
		seria::from_binary(counter, rb);
	counter += delta;
	if (counter == 1) {
		m_db.del(key, false);
	} else {
		BinaryArray ba = seria::to_binary(counter);
		m_db.put(key, ba, false);
	}
	if (counter == 0) {
		m_db.put(cd_key, std::string(), false);
	} else {
		m_db.del(cd_key, false);
	}
}

bool BlockChain::get_oldest_tip(Difficulty *cd, Hash *bid) const {
	DB::Cursor cur = m_db.begin(CD_TIPS_PREFIX);
	if (cur.end())
		return false;
	const std::string &suf = cur.get_suffix();
	const char *be         = suf.data();
	const char *en         = be + suf.size();
	*cd                     = common::read_varint_sqlite4(be, en);
	if (en - be != sizeof(bid->data))
		throw std::logic_error("CD_TIPS_PREFIX corrupted");
	DB::from_binary_key(cur.get_suffix(), cur.get_suffix().size() - sizeof(bid->data), bid->data, sizeof(bid->data));
	return true;
}

bool BlockChain::prune_branch(Difficulty cd, Hash bid) {
	if (bid == m_tip_bid)
		return false;
	check_children_counter(cd, bid, 0);
	api::BlockHeader me = read_header(bid);
	api::BlockHeader pa = read_header(me.previous_block_hash);
	modify_children_counter(cd, bid, 1);
	modify_children_counter(pa.cumulative_difficulty, me.previous_block_hash, -1);
	auto key = BLOCK_PREFIX + DB::to_binary_key(bid.data, sizeof(bid.data)) + BLOCK_SUFFIX;
	m_db.del(key, true);
	auto key2 = HEADER_PREFIX + DB::to_binary_key(bid.data, sizeof(bid.data)) + HEADER_SUFFIX;
	m_db.del(key2, true);
	return true;
}

void BlockChain::test_prune_oldest() {
	for (int i = 0; i != 10; ++i) {
		Difficulty cd = 0;
		Hash bid;
		if (!get_oldest_tip(&cd, &bid))
			return;
		prune_branch(cd, bid);
	}
}

void BlockChain::test_print_structure(Height n_confirmations) const {
	Difficulty ocd;
	Hash obid;
	if (get_oldest_tip(&ocd, &obid))
		std::cout << "oldest tip cd=" << ocd << " bid=" << common::pod_to_hex(obid) << std::endl;
	for (DB::Cursor cur = m_db.begin(CHILDREN_PREFIX); !cur.end(); cur.next()) {
		Hash bid;
		DB::from_binary_key(cur.get_suffix(), 0, bid.data, sizeof(bid.data));
		uint32_t counter = 1;
		seria::from_binary(counter, cur.get_value_array());

		std::cout << "children counter=" << counter << " bid=" << common::pod_to_hex(bid) << std::endl;
	}
	size_t total_forked_transactions      = 0;
	size_t total_possible_ds_transactions = 0;
	Amount total_possible_ds_amount       = 0;
	size_t total_forked_blocks            = 0;
	size_t total_forked_blocks_not_found  = 0;
	for (DB::Cursor cur = m_db.begin(CD_TIPS_PREFIX); !cur.end(); cur.next()) {
		const std::string &suf = cur.get_suffix();
		const char *be         = suf.data();
		const char *en         = be + suf.size();
		Difficulty cd          = common::read_varint_sqlite4(be, en);
		Hash bid;
		if (en - be != sizeof(bid.data))
			throw std::logic_error("CD_TIPS_PREFIX corrupted");
		DB::from_binary_key(cur.get_suffix(), cur.get_suffix().size() - sizeof(bid.data), bid.data, sizeof(bid.data));
		std::cout << "tip cd=" << cd << " bid=" << common::pod_to_hex(bid) << std::endl;
		Height t_height = Height(-1);
		while (true) {
			api::BlockHeader header = read_header(bid);
			if (t_height == Height(-1))
				t_height = header.height;
			Hash main_bid;
			if (read_chain(header.height, &main_bid) && main_bid == header.hash)
				break;  // Reached main trunk
			const bool confirmed = t_height >= header.height + n_confirmations;
			std::cout << "    fork height=" << header.height << " confirmed=" << confirmed
			          << " bid=" << common::pod_to_hex(bid) << std::endl;
			RawBlock rb;
			Block block;
			if (confirmed) {
				total_forked_blocks += 1;
				if (read_block(bid, &rb) && block.from_raw_block(rb)) {
					for (size_t tx_pos = 0; tx_pos != block.header.transaction_hashes.size(); ++tx_pos) {
						Hash tid = block.header.transaction_hashes.at(tx_pos);
						total_forked_transactions += 1;
						Transaction tx;
						Height height = 0;
						Hash block_hash;
						size_t index_in_block = 0;
						if (!read_transaction(tid, &tx, &height, &block_hash, &index_in_block)) {
							Amount input_amount = 0;
							for (const auto &input : block.transactions.at(tx_pos).inputs)
								if (input.type() == typeid(KeyInput)) {
									const KeyInput &in = boost::get<KeyInput>(input);
									input_amount += in.amount;
								}
							total_possible_ds_transactions += 1;
							total_possible_ds_amount += input_amount;
							std::cout << "        Potential ds tx amount=" << input_amount
							          << " tid=" << common::pod_to_hex(tid) << std::endl;
						}
					}
				} else
					total_forked_blocks_not_found += 1;
			}
			bid = header.previous_block_hash;
		}
	}
	std::cout << "n_confirmations=" << n_confirmations << std::endl;
	std::cout << "total forked blocks=" << total_forked_blocks << ", not found " << total_forked_blocks_not_found
	          << std::endl;
	std::cout << "total forked transactions=" << total_forked_transactions << ", possible ds "
	          << total_possible_ds_transactions << " total amount=" << total_possible_ds_amount << std::endl;
}

bool BlockChain::fix_consensus(Hash bid, const api::BlockHeader &was_info) {
	RawBlock rb;
	Block block;
	if (!read_block(bid, &rb))
		throw std::logic_error("Block for fix consensus not found - please delete " + m_coin_folder + "/blockchain");
	PreparedBlock pb(std::move(rb), nullptr);

	api::BlockHeader info;
	api::BlockHeader prev_info;
	prev_info.height = -1;
	if (pb.bid != m_genesis_bid)
		prev_info            = read_header(pb.block.header.previous_block_hash);
	info.major_version       = pb.block.header.major_version;
	info.minor_version       = pb.block.header.minor_version;
	info.timestamp           = pb.block.header.timestamp;
	info.previous_block_hash = pb.block.header.previous_block_hash;
	info.nonce               = pb.block.header.nonce;
	info.hash                = pb.bid;
	info.height              = prev_info.height + 1;

	std::string check_error = check_standalone_consensus(pb, &info, prev_info, true);
	if (!check_error.empty())
		throw std::runtime_error("Failed to fix consensus differences for block " + common::pod_to_hex(bid) +
		                         ", because " + check_error + ", " + delete_blockchain_message + m_coin_folder +
		                         "/blockchain");
	if (info.cumulative_difficulty != was_info.cumulative_difficulty) {
		//		std::cout << "Cumulative difficulty difference for height=" << start_height << " mustbe="
		//				  << info.cumulative_difficulty << " was=" << was_info.cumulative_difficulty << std::endl;
		auto key       = HEADER_PREFIX + DB::to_binary_key(bid.data, sizeof(bid.data)) + HEADER_SUFFIX;
		BinaryArray ba = seria::to_binary(info);
		m_db.put(key, ba, false);
		header_cache.erase(bid);
		return true;
	}
	return false;
}

void BlockChain::test_consensus(Height start_height) {
	int fixed_counter = 0;
	while (true) {
		Hash bid;
		if (!read_chain(start_height, &bid))
			break;  // Reached main trunk
		api::BlockHeader was_info = read_header(bid);
		if (start_height % 100 == 0)
			std::cout << "Testing consensus difference. Will take several minutes. height=" << start_height
			          << " fixed so far " << fixed_counter << std::endl;
		if (fix_consensus(bid, was_info))
			fixed_counter += 1;
		start_height += 1;
	}
	if (fixed_counter != 0)
		throw std::runtime_error(
		    "test_consensus fixed_counter=" + std::to_string(fixed_counter) + " will not commit changes.");
	std::cout << "Testing consesus complete, no differences" << std::endl;
}

void BlockChain::fix_difficulty_consensus() {
	//	const size_t max_ch = sizeof(difficulty_check)/sizeof(*difficulty_check);
	int fixed_counter = 0;
	std::set<Hash> fixed_headers;
	for (size_t ch = 0; ch != difficulty_check_count; ++ch) {
		Hash bid;
		if (!common::pod_from_hex(difficulty_check[ch].hash, bid))
			throw std::logic_error("DifficultyCheck table corrupted");
		api::BlockHeader was_info;
		if (!read_header(bid, &was_info))
			break;
		if (ch % 1000 == 0)
			std::cout << "Checking consensus difference. Will take several minutes. Remained blocks "
			          << difficulty_check_count - ch << " fixed so far " << fixed_counter << std::endl;
		fixed_headers.insert(bid);
		if (was_info.cumulative_difficulty != difficulty_check[ch].cumulative_difficulty) {
			if (ch == 0)  // We are not sure previous block has right difficulty, aborting
				throw std::runtime_error("Failed to fix zero consensus differences, " + delete_blockchain_message +
				                         m_coin_folder + "/blockchain");
			if (fixed_counter % 100 == 0)
				std::cout << "Fixing consensus difference for height " << difficulty_check[ch].height
				          << ". Will take several minutes. remained blocks " << difficulty_check_count - ch
				          << " fixed so far " << fixed_counter << std::endl;
			fixed_counter += 1;
			fix_consensus(bid, was_info);
			api::BlockHeader now_info = read_header(bid);
			if (now_info.cumulative_difficulty != difficulty_check[ch].cumulative_difficulty) {
				throw std::runtime_error("Failed to fix consensus differences, " + delete_blockchain_message +
				                         m_coin_folder + "/blockchain");
			}
		}
	}
	std::cout << "Checked " << difficulty_check_count << " headers for consensus differences, fixed " << fixed_counter
	          << std::endl;
	//	start_height = difficulty_check[0].height;
	//	std::cout << "Found header with consensus differences at height"<< start_height << ", fixing (can take several
	//minutes)..." << std::endl;
	int progress_counter = 0;
	for (DB::Cursor cur = m_db.rbegin(CD_TIPS_PREFIX); !cur.end(); cur.next()) {
		const std::string &suf = cur.get_suffix();
		const char *be         = suf.data();
		const char *en         = be + suf.size();
		Difficulty cd          = common::read_varint_sqlite4(be, en);
		Hash tip_bid;
		if (en - be != sizeof(tip_bid.data))
			throw std::logic_error("CD_TIPS_PREFIX corrupted");
		DB::from_binary_key(
		    cur.get_suffix(), cur.get_suffix().size() - sizeof(tip_bid.data), tip_bid.data, sizeof(tip_bid.data));
		//		if( tip_bid == get_tip_bid())
		//			std::cout << "Main chain" << std::endl;
		std::vector<Hash> side_chain;
		for (Hash bid = tip_bid;;) {
			api::BlockHeader header = read_header(bid);
			Hash main_bid;
			if (fixed_headers.count(bid) != 0 || header.height < difficulty_check[0].height)
				break;  // Do not fix beyond table start, or beyond fixed header
			side_chain.push_back(bid);
			bid = header.previous_block_hash;
		}
		std::cout << "tip cd=" << cd << " len=" << side_chain.size() << " bid=" << common::pod_to_hex(tip_bid)
		          << std::endl;
		for (; !side_chain.empty(); side_chain.pop_back()) {
			Hash bid                  = side_chain.back();
			api::BlockHeader was_info = read_header(bid);
			if (progress_counter % 100 == 0)
				std::cout << "Fixing consensus difference. Will take several minutes. Remained blocks "
				          << side_chain.size() << " fixed so far " << fixed_counter << std::endl;
			progress_counter += 1;
			if (fix_consensus(bid, was_info))
				fixed_counter += 1;
			fixed_headers.insert(bid);
		}
	}
	api::BlockHeader tip_block  = read_header(m_tip_bid);
	m_tip_cumulative_difficulty = tip_block.cumulative_difficulty;
	tip_changed();
	std::cout << "Checked consensus, made " << fixed_counter << " fixes" << std::endl;
	//	throw std::runtime_error("Preventing DB commit");
}

void BlockChain::check_consensus_fast(Hash bid){
	RawBlock rb;
	Block block;
	if (!read_block(bid, &rb))
		throw std::logic_error("Block for fix consensus not found - please delete " + m_coin_folder + "/blockchain");
	PreparedBlock pb(std::move(rb), nullptr);

	api::BlockHeader info;
	api::BlockHeader prev_info;
	prev_info.height = -1;
	if (pb.bid != m_genesis_bid)
		prev_info            = read_header(pb.block.header.previous_block_hash);
	info.major_version       = pb.block.header.major_version;
	info.minor_version       = pb.block.header.minor_version;
	info.timestamp           = pb.block.header.timestamp;
	info.previous_block_hash = pb.block.header.previous_block_hash;
	info.nonce               = pb.block.header.nonce;
	info.hash                = pb.bid;
	info.height              = prev_info.height + 1;

	std::string check_error = check_standalone_consensus(pb, &info, prev_info, false);
	if (!check_error.empty())
		throw std::runtime_error("Failed to fix consensus differences for block " + common::pod_to_hex(bid) +
								 ", because " + check_error + ", " + delete_blockchain_message + m_coin_folder +
								 "/blockchain");
}

void BlockChain::check_consensus_fast(){
	Height CHECK_DEPTH = 20000;
	std::set<Hash> fixed_headers;
	int progress_counter = 0;
	for (DB::Cursor cur = m_db.rbegin(CD_TIPS_PREFIX); !cur.end(); cur.next()) {
		const std::string &suf = cur.get_suffix();
		const char *be         = suf.data();
		const char *en         = be + suf.size();
		Difficulty cd          = common::read_varint_sqlite4(be, en);
		Hash tip_bid;
		if (en - be != sizeof(tip_bid.data))
			throw std::logic_error("CD_TIPS_PREFIX corrupted");
		DB::from_binary_key(
				cur.get_suffix(), cur.get_suffix().size() - sizeof(tip_bid.data), tip_bid.data, sizeof(tip_bid.data));
		//		if( tip_bid == get_tip_bid())
		//			std::cout << "Main chain" << std::endl;
		std::vector<Hash> side_chain;
		for (Hash bid = tip_bid;;) {
			api::BlockHeader header = read_header(bid);
			Hash main_bid;
			if (fixed_headers.count(bid) != 0 || bid == m_genesis_bid || header.height + CHECK_DEPTH < get_tip_height())
				break;  // Do not fix beyond genesis, or beyond fixed header
			side_chain.push_back(bid);
			bid = header.previous_block_hash;
		}
		std::cout << "tip cd=" << cd << " len=" << side_chain.size() << " bid=" << common::pod_to_hex(tip_bid)
				  << std::endl;
		for (; !side_chain.empty(); side_chain.pop_back()) {
			Hash bid                  = side_chain.back();
			if (progress_counter % 1000 == 0)
				std::cout << "Quickly checking database indexes. Will take minute or two. Remained blocks "
						  << side_chain.size() << std::endl; // Checking orphan tree tips
			progress_counter += 1;
			check_consensus_fast(bid);
			fixed_headers.insert(bid);
		}
	}
}

bool BlockChain::read_next_internal_block(Hash *bid) const {
	BinaryArray ba;
	if (!m_db.get(
	        TIP_CHAIN_PREFIX + previous_versions[0] + "/" + common::write_varint_sqlite4(get_tip_height() + 1), ba))
		return false;
	seria::from_binary(*bid, ba);
	return true;
}

bool BlockChain::internal_import() {
	auto idea_start = std::chrono::high_resolution_clock::now();
	while (true) {
		Hash bid;
		if (!read_next_internal_block(&bid))
			break;
		RawBlock rb;
		if (!read_block(bid, &rb))
			break;
		PreparedBlock pb(std::move(rb), nullptr);
		api::BlockHeader info;
		if (add_block(pb, &info) != BroadcastAction::BROADCAST_ALL) {
			std::cout << "internal_import block_chain.add_block !BROADCAST_ALL block=" << get_tip_height() + 1
			          << std::endl;
			break;
		}
		if (get_tip_height() % 50000 == 0)
			db_commit();
		auto idea_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		    std::chrono::high_resolution_clock::now() - idea_start);
		if (idea_ms.count() > 100)
			return true;  // continue importing
	}
	std::cout << "Finished internal importing of blocks, clearing chains..." << std::endl;
	size_t erased = 0;
	for (DB::Cursor cur = m_db.begin(TIP_CHAIN_PREFIX + previous_versions[0] + "/"); !cur.end(); cur.erase()) {
		erased += 1;
	}
	std::cout << "Items erased " << erased << std::endl;
	m_internal_import_known_height = 0;
	db_commit();
	return false;
}

void BlockChain::test_undo_everything(Height new_tip_height) {
	while (get_tip_height() > new_tip_height) {
		RawBlock raw_block;
		Block block;
		if (!read_block(get_tip_bid(), &raw_block) || !block.from_raw_block(raw_block))
			break;
		undo_block(get_tip_bid(), raw_block, block, m_tip_height);
		if (get_tip_bid() == m_genesis_bid)
			break;
		pop_chain();
		m_tip_bid                   = block.header.previous_block_hash;
		api::BlockHeader info       = get_tip();
		m_tip_cumulative_difficulty = info.cumulative_difficulty;
		tip_changed();
		if (get_tip_height() % 50000 == 1)
			db_commit();
	}
	std::cout << "---- After undo everything ---- " << std::endl;
	int counter = 0;
	for (DB::Cursor cur = m_db.begin(std::string()); !cur.end(); cur.next()) {
		if (cur.get_suffix().find(BLOCK_PREFIX) == 0)
			continue;
		if (cur.get_suffix().find(HEADER_PREFIX) == 0)
			continue;
		if (cur.get_suffix().find("f") == 0)
			continue;
		std::cout << DB::clean_key(cur.get_suffix()) << std::endl;
		if (counter++ > 1000)
			break;
	}
}
