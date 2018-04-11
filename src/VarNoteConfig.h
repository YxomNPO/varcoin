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

#include <cstddef>
#include <cstdint>
#include <limits>

namespace VarNote {
namespace parameters {

const uint32_t VARNOTE_MAX_BLOCK_NUMBER                   = 500000000;
const size_t   VARNOTE_MAX_BLOCK_BLOB_SIZE                = 500000000;
const size_t   VARNOTE_MAX_TX_SIZE                        = 1000000000;
const uint64_t VARNOTE_PUBLIC_ADDRESS_BASE58_PREFIX       = 0x3d; // addresses start with "B"
const uint32_t VARNOTE_MINED_MONEY_UNLOCK_WINDOW          = 10;
const uint64_t VARNOTE_BLOCK_FUTURE_TIME_LIMIT            = 60 * 60 * 2;

const size_t   BLOCKCHAIN_TIMESTAMP_CHECK_WINDOW             = 60;

// MONEY_SUPPLY - total number coins to be generated
const uint64_t MONEY_SUPPLY                                  = UINT64_C(210000000000000);
const unsigned EMISSION_SPEED_FACTOR                         = 21;
static_assert(EMISSION_SPEED_FACTOR <= 8 * sizeof(uint64_t), "Bad EMISSION_SPEED_FACTOR");

const size_t   VARNOTE_REWARD_BLOCKS_WINDOW               = 100;
const size_t   VARNOTE_BLOCK_GRANTED_FULL_REWARD_ZONE     = 100000; //size of block (bytes) after which reward for block calculated using block size
const size_t   VARNOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V2  = 10000;
const size_t   VARNOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1  = 10000;
const size_t   VARNOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_CURRENT = VARNOTE_BLOCK_GRANTED_FULL_REWARD_ZONE;
const size_t   VARNOTE_COINBASE_BLOB_RESERVED_SIZE        = 600;
const size_t   VARNOTE_DISPLAY_DECIMAL_POINT              = 6;
const uint64_t MINIMUM_FEE                                   = UINT64_C(1000000);    // pow(10, 6)
const uint64_t DEFAULT_DUST_THRESHOLD                        = UINT64_C(1000000);    // pow(10, 6)

const uint64_t DIFFICULTY_TARGET                             = 120; // seconds
const uint64_t EXPECTED_NUMBER_OF_BLOCKS_PER_DAY             = 24 * 60 * 60 / DIFFICULTY_TARGET;
const size_t   DIFFICULTY_WINDOW                             = EXPECTED_NUMBER_OF_BLOCKS_PER_DAY; // blocks
const size_t   DIFFICULTY_CUT                                = 60;  // timestamps to cut after sorting
const size_t   DIFFICULTY_LAG                                = 15;  // !!!
static_assert(2 * DIFFICULTY_CUT <= DIFFICULTY_WINDOW - 2, "Bad DIFFICULTY_WINDOW or DIFFICULTY_CUT");

const size_t   MAX_BLOCK_SIZE_INITIAL                        =  20 * 1024;
const uint64_t MAX_BLOCK_SIZE_GROWTH_SPEED_NUMERATOR         = 100 * 1024;
const uint64_t MAX_BLOCK_SIZE_GROWTH_SPEED_DENOMINATOR       = 365 * 24 * 60 * 60 / DIFFICULTY_TARGET;

const uint64_t VARNOTE_LOCKED_TX_ALLOWED_DELTA_BLOCKS     = 1;
const uint64_t VARNOTE_LOCKED_TX_ALLOWED_DELTA_SECONDS    = DIFFICULTY_TARGET * VARNOTE_LOCKED_TX_ALLOWED_DELTA_BLOCKS;

const uint64_t VARNOTE_MEMPOOL_TX_LIVETIME                = 60 * 60 * 24;     //seconds, one day
const uint64_t VARNOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME = 60 * 60 * 24 * 7; //seconds, one week
const uint64_t VARNOTE_NUMBER_OF_PERIODS_TO_FORGET_TX_DELETED_FROM_POOL = 7;  // VARNOTE_NUMBER_OF_PERIODS_TO_FORGET_TX_DELETED_FROM_POOL * VARNOTE_MEMPOOL_TX_LIVETIME = time to forget tx

const size_t   FUSION_TX_MAX_SIZE                            = VARNOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_CURRENT * 30 / 100;
const size_t   FUSION_TX_MIN_INPUT_COUNT                     = 12;
const size_t   FUSION_TX_MIN_IN_OUT_COUNT_RATIO              = 4;

const uint32_t UPGRADE_HEIGHT_V2                             = 15177;
const uint32_t UPGRADE_HEIGHT_V3                             = 24555;
const unsigned UPGRADE_VOTING_THRESHOLD                      = 60;               // percent
const uint32_t UPGRADE_VOTING_WINDOW                         = EXPECTED_NUMBER_OF_BLOCKS_PER_DAY;  // blocks
const uint32_t UPGRADE_WINDOW                                = EXPECTED_NUMBER_OF_BLOCKS_PER_DAY;  // blocks
static_assert(0 < UPGRADE_VOTING_THRESHOLD && UPGRADE_VOTING_THRESHOLD <= 100, "Bad UPGRADE_VOTING_THRESHOLD");
static_assert(UPGRADE_VOTING_WINDOW > 1, "Bad UPGRADE_VOTING_WINDOW");

const char     VARNOTE_BLOCKS_FILENAME[]                  = "blocks.bin";
const char     VARNOTE_BLOCKINDEXES_FILENAME[]            = "blockindexes.bin";
const char     VARNOTE_POOLDATA_FILENAME[]                = "poolstate.bin";
const char     P2P_NET_DATA_FILENAME[]                       = "p2pstate.bin";
const char     MINER_CONFIG_FILE_NAME[]                      = "miner_conf.json";
} // parameters

const char     VARNOTE_NAME[]                             = "varcoin";

const uint8_t  TRANSACTION_VERSION_1                         =  1;
const uint8_t  TRANSACTION_VERSION_2                         =  2;
const uint8_t  CURRENT_TRANSACTION_VERSION                   =  TRANSACTION_VERSION_1;
const uint8_t  BLOCK_MAJOR_VERSION_1                         =  1;
const uint8_t  BLOCK_MAJOR_VERSION_2                         =  2;
const uint8_t  BLOCK_MAJOR_VERSION_3                         =  3;
const uint8_t  BLOCK_MINOR_VERSION_0                         =  0;
const uint8_t  BLOCK_MINOR_VERSION_1                         =  1;

const size_t   BLOCKS_IDS_SYNCHRONIZING_DEFAULT_COUNT        =  10000;  //by default, blocks ids count in synchronizing
const size_t   BLOCKS_SYNCHRONIZING_DEFAULT_COUNT            =  200;    //by default, blocks count in blocks downloading
const size_t   COMMAND_RPC_GET_BLOCKS_FAST_MAX_COUNT         =  1000;

const int      P2P_DEFAULT_PORT                              =  19898;
const int      RPC_DEFAULT_PORT                              =  19998;

const size_t   P2P_LOCAL_WHITE_PEERLIST_LIMIT                =  1000;
const size_t   P2P_LOCAL_GRAY_PEERLIST_LIMIT                 =  5000;

const size_t   P2P_CONNECTION_MAX_WRITE_BUFFER_SIZE          = 16 * 1024 * 1024; // 16 MB
const uint32_t P2P_DEFAULT_CONNECTIONS_COUNT                 = 8;
const size_t   P2P_DEFAULT_WHITELIST_CONNECTIONS_PERCENT     = 70;
const uint32_t P2P_DEFAULT_HANDSHAKE_INTERVAL                = 60;            // seconds
const uint32_t P2P_DEFAULT_PACKET_MAX_SIZE                   = 50000000;      // 50000000 bytes maximum packet size
const uint32_t P2P_DEFAULT_PEERS_IN_HANDSHAKE                = 250;
const uint32_t P2P_DEFAULT_CONNECTION_TIMEOUT                = 5000;          // 5 seconds
const uint32_t P2P_DEFAULT_PING_CONNECTION_TIMEOUT           = 2000;          // 2 seconds
const uint64_t P2P_DEFAULT_INVOKE_TIMEOUT                    = 60 * 2 * 1000; // 2 minutes
const size_t   P2P_DEFAULT_HANDSHAKE_INVOKE_TIMEOUT          = 5000;          // 5 seconds
const char     P2P_STAT_TRUSTED_PUB_KEY[] = "fffff9a5a434a9f1510000006228debfee9c918ce505efe225d8c94d045fffff";

const char* const SEED_NODES[] = {
        "seed.varcoin.info:19898",
        "seed.varcoin.net:19898"};

struct CheckpointData {
  uint32_t index;
  const char* blockId;
};

const CheckpointData CHECKPOINTS[] = {
  {1    ,
"d1ceea4ceb9236be0549ac0af1b3af980cd729c867044d9ce76c0f479b6a0b95"},
  {714  , "e9d7165c18cd3a454984bafedb78b70ba98f589f694e750cdf0d09a52c2f9ffd"},
  {1000 , "37a5f208309554cf882b72cdb9c7d0d8df367461b7eacec5f9f21d9951be73b0"},
  {2000 , "380670a14712b359ed42ce593709fe441173a01a62418043d343bff2e04a23ca"},
  {2193 , "c5b3de47c8c10be2c8b98fa655926fb9021444263b6766e63f622e8ff3de5c54"},
  {3000 , "f8e5c3b9aa3884302de3c9be3a87326b3d999769074fd2b55fb0c091d450e493"},
  {4000 , "e9e63092b93d9c5caf7ea246746c14a47d324a6f4af76c43ac9dd765e3fc9522"},
  {5000 , "5669b98217fb4ae163f4e140635098581a7295a6e550e6c3ab50fc8d09d01fdf"},
  {6000 , "3a211c89fe2713327267b869fab94fd49169b33dbeb452b3d49e1d7af14b0438"},
  {7000 , "52d504ffb9e2b6ab6bb7b3edae86057635381381604833213d2261b6ef26696c"},
  {8000 , "133e15b049bb3fd684f7cf2175225edbad635b4ced06afa6b309eb29849fb0f0"},
  {9000 , "8a899736de6934fa8e22767c465456e2c96d1be64fc658cbf76bfaabd73d4a64"},
  {10000, "59621ba8e04c36d2ebdd1eecddfd923e8a56adc1918c9414a66a5ada552c1728"},
  {20000, "429435b26569637084681ca48738318039965d663d0e03f1a544fa8663caca46"}
};
} // VarNote

#define ALLOW_DEBUG_COMMANDS
