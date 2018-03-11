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

//#include "gtest/gtest.h"
//
//#include <sstream>
//#include <limits>
//#include <boost/variant/get.hpp>
//
//#include "Serialization/BinaryOutputStreamSerializer.h"
//#include "Serialization/BinaryInputStreamSerializer.h"
//
//#include "Serialization/serialization.h"
//#include "Serialization/binary_utils.h"
//
//#include "VarNoteCore/VarNoteBasic.h"
//#include "VarNoteCore/VarNoteBasicImpl.h"
//#include "VarNoteCore/VarNoteSerialization.h"
//#include "VarNoteCore/VarNoteFormatUtils.h"
//#include "serialization_structs_comparators.h"
//
//#include <iostream>
//
//#include <boost/functional/hash.hpp>
//
//template <typename Struct>
//void checkEqualBinary(Struct& original) {
//  std::stringstream newStream;
//  std::stringstream oldStream;
//
//  VarNote::BinaryOutputStreamSerializer binarySerializer(newStream);
//  // binarySerializer(original, "");
//  VarNote::serialize(original, binarySerializer);
//
//  binary_archive<true> ba(oldStream);
//  bool r = ::serialization::serialize(ba, original);
//  ASSERT_TRUE(r);
//
//  ASSERT_EQ(oldStream.str(), newStream.str());
//}
//
//template <typename Struct>
//void checkEnumeratorToLegacy(Struct& original) {
//  std::stringstream archive;
//
//  VarNote::BinaryOutputStreamSerializer binarySerializer(archive);
//  VarNote::serialize(original, binarySerializer);
//
//  Struct restored;
//  binary_archive<false> ba(archive);
//  bool r = ::serialization::serialize(ba, restored);
//  ASSERT_TRUE(r);
//
//  ASSERT_EQ(original, restored);
//}
//
//template <typename Struct>
//void checkLegacyToEnumerator(Struct& original) {
//  std::stringstream archive;
//
//  binary_archive<true> ba(archive);
//  bool r = ::serialization::serialize(ba, original);
//  ASSERT_TRUE(r);
//
//  Struct restored;
//
//  VarNote::BinaryInputStreamSerializer binarySerializer(archive);
//  binarySerializer(restored, "");
//
//  ASSERT_EQ(original, restored);
//}
//
//template <typename Struct>
//void checkEnumeratorToEnumerator(Struct& original) {
//  std::stringstream archive;
//
//  VarNote::BinaryOutputStreamSerializer output(archive);
//  output(original, "");
//
//  Struct restored;
//  VarNote::BinaryInputStreamSerializer input(archive);
//  input(restored, "");
//
//  ASSERT_EQ(original, restored);
//}
//
//template <typename Struct>
//void checkCompatibility(Struct& original) {
//  checkEqualBinary(original);
//  ASSERT_NO_FATAL_FAILURE(checkEnumeratorToEnumerator(original));
//  ASSERT_NO_FATAL_FAILURE(checkEnumeratorToLegacy(original));
//  ASSERT_NO_FATAL_FAILURE(checkLegacyToEnumerator(original));
//}
//
//void fillData(char* data, size_t size, char startByte) {
//  for (size_t i = 0; i < size; ++i) {
//    data[i] = startByte++;
//  }
//}
//
//void fillPublicKey(Crypto::PublicKey& key, char startByte = 120) {
//  fillData(reinterpret_cast<char *>(&key), sizeof(Crypto::PublicKey), startByte);
//}
//
//void fillHash(Crypto::Hash& hash, char startByte = 120) {
//  fillData(reinterpret_cast<char *>(&hash), sizeof(Crypto::Hash), startByte);
//}
//
//void fillKeyImage(Crypto::KeyImage& image, char startByte = 120) {
//  fillData(reinterpret_cast<char *>(&image), sizeof(Crypto::KeyImage), startByte);
//}
//
//void fillSignature(Crypto::Signature& sig, char startByte = 120) {
//  fillData(reinterpret_cast<char *>(&sig), sizeof(Crypto::Signature), startByte);
//}
//
//void fillTransactionOutputMultisignature(VarNote::TransactionOutputMultisignature& s) {
//  Crypto::PublicKey key;
//  fillPublicKey(key, 0);
//  s.keys.push_back(key);
//
//  char start = 120;
//
//  fillPublicKey(key, start++);
//  s.keys.push_back(key);
//
//  fillPublicKey(key, start++);
//  s.keys.push_back(key);
//
//  fillPublicKey(key, start++);
//  s.keys.push_back(key);
//
//  fillPublicKey(key, start++);
//  s.keys.push_back(key);
//
//  fillPublicKey(key, start++);
//  s.keys.push_back(key);
//
//  s.requiredSignatures = 12;
//}
//
//void fillTransaction(VarNote::Transaction& tx) {
//  tx.version = 1;
//  tx.unlockTime = 0x7f1234560089ABCD;
//
//  VarNote::TransactionInputGenerate gen;
//  gen.height = 0xABCDEF12;
//  tx.vin.push_back(gen);
//
//  VarNote::TransactionInputToKey key;
//  key.amount = 500123;
//  key.keyOffsets = {12,3323,0x7f0000000000, std::numeric_limits<uint64_t>::max(), 0};
//  fillKeyImage(key.keyImage);
//  tx.vin.push_back(key);
//
//  VarNote::TransactionInputMultisignature multisig;
//  multisig.amount = 490000000;
//  multisig.outputIndex = 424242;
//  multisig.signatures = 4;
//  tx.vin.push_back(multisig);
//
//  VarNote::TransactionOutput txOutput;
//  txOutput.amount = 0xfff000ffff778822;
//  VarNote::TransactionOutputToKey out;
//  fillPublicKey(out.key);
//  txOutput.target = out;
//  tx.vout.push_back(txOutput);
//
//  tx.extra = {1,2,3,127,0,128,255};
//
//  tx.signatures.resize(3);
//
//  for (size_t i = 0; i < boost::get<VarNote::TransactionInputToKey>(tx.vin[1]).keyOffsets.size(); ++i) {
//    Crypto::Signature sig;
//    fillSignature(sig, static_cast<char>(i));
//    tx.signatures[1].push_back(sig);
//  }
//
//  for (size_t i = 0; i < boost::get<VarNote::TransactionInputMultisignature>(tx.vin[2]).signatures; ++i) {
//    Crypto::Signature sig;
//    fillSignature(sig, static_cast<char>(i + 120));
//    tx.signatures[2].push_back(sig);
//  }
//}
//
//void fillParentBlock(VarNote::ParentBlock& pb) {
//  pb.majorVersion = 1;
//  pb.minorVersion = 1;
//
//  fillHash(pb.prevId, 120);
//
//  pb.numberOfTransactions = 3;
//  size_t branchSize = Crypto::tree_depth(pb.numberOfTransactions);
//  for (size_t i = 0; i < branchSize; ++i) {
//    Crypto::Hash hash;
//    fillHash(hash, static_cast<char>(i));
//    pb.minerTxBranch.push_back(hash);
//  }
//
//  fillTransaction(pb.minerTx);
//
//  VarNote::tx_extra_merge_mining_tag mmTag;
//  mmTag.depth = 10;
//  fillHash(mmTag.merkle_root);
//  pb.minerTx.extra.clear();
//  VarNote::append_mm_tag_to_extra(pb.minerTx.extra, mmTag);
//
//  std::string my;
//  std::copy(pb.minerTx.extra.begin(), pb.minerTx.extra.end(), std::back_inserter(my));
//
//  for (size_t i = 0; i < mmTag.depth; ++i) {
//    Crypto::Hash hash;
//    fillHash(hash, static_cast<char>(i));
//    pb.blockchainBranch.push_back(hash);
//  }
//}
//
//void fillBlockHeaderVersion1(VarNote::BlockHeader& header) {
//  header.majorVersion = 1;
//  header.minorVersion = 1;
//  header.nonce = 0x807F00AB;
//  header.timestamp = 1408106672;
//  fillHash(header.prevId);
//}
//
//void fillBlockHeaderVersion2(VarNote::BlockHeader& header) {
//  fillBlockHeaderVersion1(header);
//  header.majorVersion = 2;
//}
//
//TEST(BinarySerializationCompatibility, TransactionOutputMultisignature) {
//  VarNote::TransactionOutputMultisignature s;
//
//  fillTransactionOutputMultisignature(s);
//
//  checkCompatibility(s);
//}
//
//TEST(BinarySerializationCompatibility, TransactionInputGenerate) {
//  VarNote::TransactionInputGenerate s;
//  s.height = 0x80000001;
//  checkCompatibility(s);
//
//  s.height = 0x7FFFFFFF;
//  checkCompatibility(s);
//
//  s.height = 0;
//  checkCompatibility(s);
//};
//
//TEST(BinarySerializationCompatibility, TransactionInputToKey) {
//  VarNote::TransactionInputToKey s;
//
//  s.amount = 123456987032;
//  s.keyOffsets = {12,3323,0x7f00000000000000, std::numeric_limits<uint64_t>::max(), 0};
//  fillKeyImage(s.keyImage);
//
//  checkCompatibility(s);
//}
//
//TEST(BinarySerializationCompatibility, TransactionInputMultisignature) {
//  VarNote::TransactionInputMultisignature s;
//  s.amount = 0xfff000ffff778822;
//  s.signatures = 0x7f259200;
//  s.outputIndex = 0;
//
//  checkCompatibility(s);
//}
//
//TEST(BinarySerializationCompatibility, TransactionOutput_TransactionOutputToKey) {
//  VarNote::TransactionOutput s;
//  s.amount = 0xfff000ffff778822;
//
//  VarNote::TransactionOutputToKey out;
//  fillPublicKey(out.key);
//  s.target = out;
//
//  checkCompatibility(s);
//}
//
//TEST(BinarySerializationCompatibility, TransactionOutput_TransactionOutputMultisignature) {
//  VarNote::TransactionOutput s;
//  s.amount = 0xfff000ffff778822;
//
//  VarNote::TransactionOutputMultisignature out;
//  fillTransactionOutputMultisignature(out);
//  s.target = out;
//
//  checkCompatibility(s);
//}
//
//TEST(BinarySerializationCompatibility, Transaction) {
//  VarNote::Transaction tx;
//
//  fillTransaction(tx);
//
//  checkCompatibility(tx);
//}
//
//TEST(BinarySerializationCompatibility, TransactionHash) {
//  VarNote::Transaction tx;
//  fillTransaction(tx);
//
//  auto h1 = VarNote::getObjectHash(tx);
//
//  std::string blob;
//  serialization::dump_binary(tx, blob);
//  Crypto::Hash h2;
//  VarNote::get_blob_hash(blob, h2);
//
//
//  auto s1 = std::hash<Crypto::Hash>()(h1);
//  auto s2 = boost::hash<Crypto::Hash>()(h1);
//
//  ASSERT_EQ(h1, h2);
//}
//
//
//void compareParentBlocks(VarNote::ParentBlock& pb, VarNote::ParentBlock& restoredPb, bool headerOnly) {
//  EXPECT_EQ(pb.majorVersion, restoredPb.majorVersion);
//  EXPECT_EQ(pb.minorVersion, restoredPb.minorVersion);
//  EXPECT_EQ(pb.prevId, restoredPb.prevId);
//
//  if (headerOnly) {
//    return;
//  }
//
//  EXPECT_EQ(pb.numberOfTransactions, restoredPb.numberOfTransactions);
//  EXPECT_EQ(pb.minerTxBranch, restoredPb.minerTxBranch);
//  EXPECT_EQ(pb.minerTx, restoredPb.minerTx);
//  EXPECT_EQ(pb.blockchainBranch, restoredPb.blockchainBranch);
//}
//
//void checkEnumeratorToLegacy(VarNote::ParentBlock& pb, uint64_t ts, uint32_t nonce, bool hashingSerialization, bool headerOnly) {
//  std::stringstream archive;
//
//  VarNote::ParentBlockSerializer original(pb, ts, nonce, hashingSerialization, headerOnly);
//  VarNote::BinaryOutputStreamSerializer output(archive);
//  output(original, "");
//
//  VarNote::ParentBlock restoredPb;
//  uint64_t restoredTs;
//  uint32_t restoredNonce;
//
//  VarNote::ParentBlockSerializer restored(restoredPb, restoredTs, restoredNonce, hashingSerialization, headerOnly);
//  binary_archive<false> ba(archive);
//  bool r = ::serialization::serialize(ba, restored);
//  ASSERT_TRUE(r);
//
//  EXPECT_EQ(nonce, restoredNonce);
//  EXPECT_EQ(ts, restoredTs);
//
//  ASSERT_NO_FATAL_FAILURE(compareParentBlocks(pb, restoredPb, headerOnly));
//}
//
//void checkLegacyToEnumerator(VarNote::ParentBlock& pb, uint64_t ts, uint32_t nonce, bool hashingSerialization, bool headerOnly) {
//  std::stringstream archive;
//
//  VarNote::ParentBlockSerializer original(pb, ts, nonce, hashingSerialization, headerOnly);
//  binary_archive<true> ba(archive);
//  bool r = ::serialization::serialize(ba, original);
//  ASSERT_TRUE(r);
//
//  VarNote::ParentBlock restoredPb;
//  uint64_t restoredTs;
//  uint32_t restoredNonce;
//
//  VarNote::ParentBlockSerializer restored(restoredPb, restoredTs, restoredNonce, hashingSerialization, headerOnly);
//
//  VarNote::BinaryInputStreamSerializer input(archive);
//  input(restored, "");
//
//  EXPECT_EQ(nonce, restoredNonce);
//  EXPECT_EQ(ts, restoredTs);
//
//  ASSERT_NO_FATAL_FAILURE(compareParentBlocks(pb, restoredPb, headerOnly));
//}
//
//void checkEnumeratorToEnumerator(VarNote::ParentBlock& pb, uint64_t ts, uint32_t nonce, bool hashingSerialization, bool headerOnly) {
//  std::stringstream archive;
//
//  VarNote::ParentBlockSerializer original(pb, ts, nonce, hashingSerialization, headerOnly);
//  VarNote::BinaryOutputStreamSerializer output(archive);
//  output(original, "");
//
//  VarNote::ParentBlock restoredPb;
//  uint64_t restoredTs;
//  uint32_t restoredNonce;
//
//  VarNote::ParentBlockSerializer restored(restoredPb, restoredTs, restoredNonce, hashingSerialization, headerOnly);
//
//  VarNote::BinaryInputStreamSerializer input(archive);
//  input(restored, "");
//
//  EXPECT_EQ(nonce, restoredNonce);
//  EXPECT_EQ(ts, restoredTs);
//
//  ASSERT_NO_FATAL_FAILURE(compareParentBlocks(pb, restoredPb, headerOnly));
//}
//
//void checkCompatibility(VarNote::ParentBlock& pb, uint64_t ts, uint32_t nonce, bool hashingSerialization, bool headerOnly) {
//  ASSERT_NO_FATAL_FAILURE(checkEnumeratorToEnumerator(pb, ts, nonce, hashingSerialization, headerOnly));
//  ASSERT_NO_FATAL_FAILURE(checkEnumeratorToLegacy(pb, ts, nonce, hashingSerialization, headerOnly));
//  ASSERT_NO_FATAL_FAILURE(checkLegacyToEnumerator(pb, ts, nonce, hashingSerialization, headerOnly));
//}
//
//TEST(BinarySerializationCompatibility, ParentBlockSerializer) {
//  VarNote::ParentBlock pb;
//  fillParentBlock(pb);
//  uint64_t timestamp = 1408106672;
//  uint32_t nonce = 1234567;
//
//  checkCompatibility(pb, timestamp, nonce, false, false);
//  checkCompatibility(pb, timestamp, nonce, true, false);
//  checkCompatibility(pb, timestamp, nonce, false, true);
//}
//
//void compareBlocks(VarNote::Block& block, VarNote::Block& restoredBlock) {
//  ASSERT_EQ(block.majorVersion, restoredBlock.majorVersion);
//  ASSERT_EQ(block.minorVersion, restoredBlock.minorVersion);
//  if (block.majorVersion == VarNote::BLOCK_MAJOR_VERSION_1) {
//    ASSERT_EQ(block.timestamp, restoredBlock.timestamp);
//    ASSERT_EQ(block.prevId, restoredBlock.prevId);
//    ASSERT_EQ(block.nonce, restoredBlock.nonce);
//  } else if (block.majorVersion == VarNote::BLOCK_MAJOR_VERSION_2) {
//    ASSERT_EQ(block.prevId, restoredBlock.prevId);
//    ASSERT_NO_FATAL_FAILURE(compareParentBlocks(block.parentBlock, restoredBlock.parentBlock, false));
//  } else {
//    throw std::runtime_error("Unknown major block version. Check your test");
//  }
//  ASSERT_EQ(block.minerTx, restoredBlock.minerTx);
//  ASSERT_EQ(block.txHashes, restoredBlock.txHashes);
//}
//
//void checkEnumeratorToLegacy(VarNote::Block& block) {
//  std::stringstream archive;
//
//  VarNote::BinaryOutputStreamSerializer output(archive);
//  output(block, "");
//
//  VarNote::Block restoredBlock;
//
//  binary_archive<false> ba(archive);
//  bool r = ::serialization::serialize(ba, restoredBlock);
//  ASSERT_TRUE(r);
//
//  ASSERT_NO_FATAL_FAILURE(compareBlocks(block, restoredBlock));
//}
//
//void checkLegacyToEnumerator(VarNote::Block& block) {
//  std::stringstream archive;
//
//  binary_archive<true> ba(archive);
//  bool r = ::serialization::serialize(ba, block);
//  ASSERT_TRUE(r);
//
//  VarNote::Block restoredBlock;
//
//  VarNote::BinaryInputStreamSerializer output(archive);
//  output(restoredBlock, "");
//
//  ASSERT_NO_FATAL_FAILURE(compareBlocks(block, restoredBlock));
//}
//
//void checkEnumeratorToEnumerator(VarNote::Block& block) {
//  std::stringstream archive;
//
//  VarNote::BinaryOutputStreamSerializer output(archive);
//  output(block, "");
//
//  VarNote::Block restoredBlock;
//
//  VarNote::BinaryInputStreamSerializer input(archive);
//  input(restoredBlock, "");
//
//  ASSERT_NO_FATAL_FAILURE(compareBlocks(block, restoredBlock));
//}
//
//void checkCompatibility(VarNote::Block& block) {
//  ASSERT_NO_FATAL_FAILURE(checkEnumeratorToEnumerator(block));
//  ASSERT_NO_FATAL_FAILURE(checkEnumeratorToLegacy(block));
//  ASSERT_NO_FATAL_FAILURE(checkLegacyToEnumerator(block));
//}
//
//TEST(BinarySerializationCompatibility, BlockVersion1) {
//  VarNote::Block block;
//  fillBlockHeaderVersion1(block);
//  fillParentBlock(block.parentBlock);
//  fillTransaction(block.minerTx);
//
//  for (size_t i = 0; i < 7; ++i) {
//    Crypto::Hash hash;
//    fillHash(hash, static_cast<char>(0x7F + i));
//    block.txHashes.push_back(hash);
//  }
//
//  checkCompatibility(block);
//}
//
//TEST(BinarySerializationCompatibility, BlockVersion2) {
//  VarNote::Block block;
//  fillBlockHeaderVersion2(block);
//  fillParentBlock(block.parentBlock);
//  fillTransaction(block.minerTx);
//
//  for (size_t i = 0; i < 7; ++i) {
//    Crypto::Hash hash;
//    fillHash(hash, static_cast<char>(0x7F + i));
//    block.txHashes.push_back(hash);
//  }
//
//  checkCompatibility(block);
//}
//
//TEST(BinarySerializationCompatibility, account_public_address) {
//  VarNote::AccountPublicAddress addr;
//
//  fillPublicKey(addr.m_spendPublicKey, '\x50');
//  fillPublicKey(addr.m_viewPublicKey, '\xAA');
//
//  checkCompatibility(addr);
//}
//
////TEST(BinarySerializationCompatibility, tx_extra_merge_mining_tag) {
////  VarNote::tx_extra_merge_mining_tag tag;
////  tag.depth = 0xdeadbeef;
////  fillHash(tag.merkle_root);
////
////  checkCompatibility(tag);
////}
//
//TEST(BinarySerializationCompatibility, readFromEmptyStream) {
//  VarNote::TransactionOutput t;
//  std::stringstream emptyStream;
//  VarNote::BinaryInputStreamSerializer s(emptyStream);
//
//  ASSERT_ANY_THROW(s(t, ""));
//}
//
//TEST(BinarySerializationCompatibility, writeToBadStream) {
//  VarNote::TransactionOutput t;
//  std::stringstream badStream;
//  VarNote::BinaryOutputStreamSerializer s(badStream);
//
//  badStream.setstate(std::ios::badbit);
//  ASSERT_ANY_THROW(s(t, ""));
//}
