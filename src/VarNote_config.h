// Copyright (c) 2017-2018 YxomTech
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#define VarNote_MAX_BLOCK_NUMBER                        500000000
#define VarNote_MAX_BLOCK_SIZE                          500000000  // block header blob limit, never used!
#define VarNote_MAX_TX_SIZE                             1000000000
#define VarNote_PUBLIC_ADDRESS_TEXTBLOB_VER             0
// Currency-specific address prefix "B"
#define VarNote_PUBLIC_ADDRESS_BASE58_PREFIX            0x3d
// Choose maturity period for your currency
#define VarNote_MINED_MONEY_UNLOCK_WINDOW               10
#define CURRENT_TRANSACTION_VERSION                     1
#define CURRENT_BLOCK_MAJOR_VERSION                     1
#define CURRENT_BLOCK_MINOR_VERSION                     0
#define VarNote_BLOCK_FUTURE_TIME_LIMIT                 60 * 60 * 2

#define BLOCKCHAIN_TIMESTAMP_CHECK_WINDOW               60

// Specify total number of available coins
//((uint64_t)(-1)) equals to 18446744073709551616 coins
//UINT64_C(18446744073709551616)
#define MONEY_SUPPLY                                    UINT64_C(210000000000000)
#define EMISSION_SPEED_FACTOR                           21

// Define number of blocks for block size median calculation
#define VarNote_REWARD_BLOCKS_WINDOW                    100
#define VarNote_BLOCK_GRANTED_FULL_REWARD_ZONE          10000 //size of block (bytes) after which reward for block calculated using block size
#define VarNote_COINBASE_BLOB_RESERVED_SIZE             600
// Define number of digits
#define VarNote_DISPLAY_DECIMAL_POINT                   6
// Define number of smallest units in one coin
#define COIN                                            1000000
// Define default fee for transactions
#define DEFAULT_FEE                                     10000

// Define preferred block's target time
#define DIFFICULTY_TARGET                               120 // seconds
// There are options to tune VarNote's difficulty retargeting function. 
// We recommend not to change it.
#define DIFFICULTY_WINDOW                               720 // blocks
#define DIFFICULTY_LAG                                  15
#define DIFFICULTY_CUT                                  60  // timestamps to cut after sorting
#define DIFFICULTY_BLOCKS_COUNT                         DIFFICULTY_WINDOW + DIFFICULTY_LAG


#define VarNote_LOCKED_TX_ALLOWED_DELTA_SECONDS         DIFFICULTY_TARGET * VarNote_LOCKED_TX_ALLOWED_DELTA_BLOCKS
#define VarNote_LOCKED_TX_ALLOWED_DELTA_BLOCKS          1


#define DIFFICULTY_BLOCKS_ESTIMATE_TIMESPAN             DIFFICULTY_TARGET


#define BLOCKS_IDS_SYNCHRONIZING_DEFAULT_COUNT          10000  //by default, blocks ids count in synchronizing
#define BLOCKS_SYNCHRONIZING_DEFAULT_COUNT              200    //by default, blocks count in blocks downloading
#define VarNote_PROTOCOL_HOP_RELAX_COUNT                3      //value of hop, after which we use only announce of new block

//TODO This port will be used by the daemon to establish connections with p2p network
#define P2P_DEFAULT_PORT                                19898
//TODO This port will be used by the daemon to interact with simlewallet
#define RPC_DEFAULT_PORT                                19998
#define COMMAND_RPC_GET_BLOCKS_FAST_MAX_COUNT           1000

#define P2P_LOCAL_WHITE_PEERLIST_LIMIT                  1000
#define P2P_LOCAL_GRAY_PEERLIST_LIMIT                   5000

#define P2P_DEFAULT_CONNECTIONS_COUNT                   8
#define P2P_DEFAULT_HANDSHAKE_INTERVAL                  60             //secondes
#define P2P_DEFAULT_PACKET_MAX_SIZE                     50000000       //50000000 bytes maximum packet size
#define P2P_DEFAULT_PEERS_IN_HANDSHAKE                  250
#define P2P_DEFAULT_CONNECTION_TIMEOUT                  5000           //5 seconds
#define P2P_DEFAULT_PING_CONNECTION_TIMEOUT             2000           //2 seconds
#define P2P_DEFAULT_INVOKE_TIMEOUT                      60 * 2 * 1000  //2 minutes
#define P2P_DEFAULT_HANDSHAKE_INVOKE_TIMEOUT            5000           //5 seconds
#define P2P_STAT_TRUSTED_PUB_KEY                        "fffff9a5a434a9f1510000006228debfee9c918ce505efe225d8c94d045fffff"
#define P2P_DEFAULT_WHITELIST_CONNECTIONS_PERCENT       70

#define ALLOW_DEBUG_COMMANDS

// Put here the name of your currency
#define VarNote_NAME                         "varcoin"
#define VarNote_BLOCKS_FILENAME              "blocks.dat"
#define VarNote_BLOCKINDEXES_FILENAME        "blockindexes.dat"
#define VarNote_BLOCKSCACHE_FILENAME         "blockscache.dat"
#define VarNote_POOLDATA_FILENAME            "poolstate.bin"
#define P2P_NET_DATA_FILENAME                "p2pstate.bin"
#define MINER_CONFIG_FILE_NAME               "miner_conf.json"

#define THREAD_STACK_SIZE                    5 * 1024 * 1024

#define GENESIS_COINBASE_TX_HEX "010a01ff0001fbe6df2f029b2e4c0281c0b02e7c53291a94d1d0cbff8883f8024f5142ee494ffbbd0880712101067f102abfe1f8d499220733fe38b3ddf93241ae4720936f113fe12961ca4bfe"
