// Copyright (c) 2012-2018, The CryptoNote developers, YxomTech.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#include "MiningConfig.hpp"
#include "common/CommandLine.hpp"
#include "common/Ipv4Address.hpp"

#include <iostream>
#include <thread>

#include "VarNoteConfig.hpp"
#include "logging/ILogger.hpp"

using namespace varcoin;

MiningConfig::MiningConfig(common::CommandLine &cmd)
    : varcoind_ip("127.0.0.1"), varcoind_port(RPC_DEFAULT_PORT), thread_count(std::thread::hardware_concurrency()) {
	if (const char *pa = cmd.get("--address"))
		mining_address = pa;
	if (const char *pa = cmd.get("--varcoind-address")) {
		if (!common::parse_ip_address_and_port(pa, &varcoind_ip, &varcoind_port))
			throw std::runtime_error("Wrong address format " + std::string(pa) + ", should be ip:port");
	}
	if (const char *pa = cmd.get("--daemon-address", "Use --varcoind-address instead")) {
		if (!common::parse_ip_address_and_port(pa, &varcoind_ip, &varcoind_port))
			throw std::runtime_error("Wrong address format " + std::string(pa) + ", should be ip:port");
	}
	if (const char *pa = cmd.get("--daemon-host", "Use --varcoind-address instead"))
		varcoind_ip = pa;
	if (const char *pa = cmd.get("--daemon-rpc-port", "Use --varcoind-address instead"))
		varcoind_port = boost::lexical_cast<uint16_t>(pa);
	if (const char *pa = cmd.get("--threads"))
		thread_count = boost::lexical_cast<size_t>(pa);
	if (const char *pa = cmd.get("--limit"))
		blocks_limit = boost::lexical_cast<size_t>(pa);
}
