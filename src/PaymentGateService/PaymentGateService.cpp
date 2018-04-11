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

#include "PaymentGateService.h"

#include <future>

#include "Common/SignalHandler.h"
#include "Common/Util.h"
#include "InProcessNode/InProcessNode.h"
#include "Logging/LoggerRef.h"
#include "PaymentGate/PaymentServiceJsonRpcServer.h"

#include "Common/ScopeExit.h"
#include "VarNoteCore/Core.h"
#include "VarNoteCore/DatabaseBlockchainCacheFactory.h"
#include "VarNoteCore/DataBaseConfig.h"
#include "VarNoteCore/MainChainStorage.h"
#include "VarNoteCore/RocksDBWrapper.h"
#include "VarNoteProtocol/VarNoteProtocolHandler.h"
#include "P2p/NetNode.h"
#include <System/Context.h>
#include "Wallet/WalletGreen.h"

#ifdef ERROR
#undef ERROR
#endif

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

using namespace PaymentService;

void changeDirectory(const std::string& path) {
  if (chdir(path.c_str())) {
    throw std::runtime_error("Couldn't change directory to \'" + path + "\': " + strerror(errno));
  }
}

void stopSignalHandler(PaymentGateService* pg) {
  pg->stop();
}

PaymentGateService::PaymentGateService() :
  dispatcher(nullptr),
  stopEvent(nullptr),
  config(),
  service(nullptr),
  logger(),
  currencyBuilder(logger),
  fileLogger(Logging::TRACE),
  consoleLogger(Logging::INFO) {
  consoleLogger.setPattern("%D %T %L ");
  fileLogger.setPattern("%D %T %L ");
}

bool PaymentGateService::init(int argc, char** argv) {
  if (!config.init(argc, argv)) {
    return false;
  }

  logger.setMaxLevel(static_cast<Logging::Level>(config.gateConfiguration.logLevel));
  logger.setPattern("%D %T %L ");
  logger.addLogger(consoleLogger);

  Logging::LoggerRef log(logger, "main");

  if (config.gateConfiguration.testnet) {
    log(Logging::INFO) << "Starting in testnet mode";
    currencyBuilder.testnet(true);
  }

  if (!config.gateConfiguration.serverRoot.empty()) {
    changeDirectory(config.gateConfiguration.serverRoot);
    log(Logging::INFO) << "Current working directory now is " << config.gateConfiguration.serverRoot;
  }

  fileStream.open(config.gateConfiguration.logFile, std::ofstream::app);

  if (!fileStream) {
    throw std::runtime_error("Couldn't open log file");
  }

  fileLogger.attachToStream(fileStream);
  logger.addLogger(fileLogger);

  return true;
}

WalletConfiguration PaymentGateService::getWalletConfig() const {
  return WalletConfiguration{
    config.gateConfiguration.containerFile,
    config.gateConfiguration.containerPassword
  };
}

const VarNote::Currency PaymentGateService::getCurrency() {
  return currencyBuilder.currency();
}

void PaymentGateService::run() {
  
  System::Dispatcher localDispatcher;
  System::Event localStopEvent(localDispatcher);

  this->dispatcher = &localDispatcher;
  this->stopEvent = &localStopEvent;

  Tools::SignalHandler::install(std::bind(&stopSignalHandler, this));

  Logging::LoggerRef log(logger, "run");

  if (config.startInprocess) {
    runInProcess(log);
  } else {
    runRpcProxy(log);
  }

  this->dispatcher = nullptr;
  this->stopEvent = nullptr;
}

void PaymentGateService::stop() {
  Logging::LoggerRef log(logger, "stop");

  log(Logging::INFO, Logging::BRIGHT_WHITE) << "Stop signal caught";

  if (dispatcher != nullptr) {
    dispatcher->remoteSpawn([&]() {
      if (stopEvent != nullptr) {
        stopEvent->set();
      }
    });
  }
}

void PaymentGateService::runInProcess(Logging::LoggerRef& log) {
  log(Logging::INFO) << "Starting Payment Gate with local node";

  VarNote::DataBaseConfig dbConfig;

  //TODO: make command line options
  dbConfig.setConfigFolderDefaulted(true);
  dbConfig.setDataDir(config.dataDir);
  dbConfig.setMaxOpenFiles(10);
  dbConfig.setReadCacheSize(100*1024*1024);
  dbConfig.setWriteBufferSize(100*1024*1024);
  dbConfig.setTestnet(false);
  dbConfig.setBackgroundThreadsCount(2);

  if (dbConfig.isConfigFolderDefaulted()) {
    if (!Tools::create_directories_if_necessary(dbConfig.getDataDir())) {
      throw std::runtime_error("Can't create directory: " + dbConfig.getDataDir());
    }
  } else {
    if (!Tools::directoryExists(dbConfig.getDataDir())) {
      throw std::runtime_error("Directory does not exist: " + dbConfig.getDataDir());
    }
  }

  VarNote::RocksDBWrapper database(logger);
  database.init(dbConfig);
  Tools::ScopeExit dbShutdownOnExit([&database] () { database.shutdown(); });

  VarNote::Currency currency = currencyBuilder.currency();

  log(Logging::INFO) << "initializing core";

  VarNote::Core core(
    currency,
    logger,
    VarNote::Checkpoints(logger),
    *dispatcher,
    std::unique_ptr<VarNote::IBlockchainCacheFactory>(new VarNote::DatabaseBlockchainCacheFactory(database, log.getLogger())),
    VarNote::createSwappedMainChainStorage(dbConfig.getDataDir(), currency));

  core.load();

  VarNote::VarNoteProtocolHandler protocol(currency, *dispatcher, core, nullptr, logger);
  VarNote::NodeServer p2pNode(*dispatcher, protocol, logger);

  protocol.set_p2p_endpoint(&p2pNode);

  log(Logging::INFO) << "initializing p2pNode";
  if (!p2pNode.init(config.netNodeConfig)) {
    throw std::runtime_error("Failed to init p2pNode");
  }

  std::unique_ptr<VarNote::INode> node(new VarNote::InProcessNode(core, protocol, *dispatcher));

  std::error_code nodeInitStatus;
  node->init([&log, &nodeInitStatus](std::error_code ec) {
    nodeInitStatus = ec;
  });

  if (nodeInitStatus) {
    log(Logging::WARNING, Logging::YELLOW) << "Failed to init node: " << nodeInitStatus.message();
    throw std::system_error(nodeInitStatus);
  } else {
    log(Logging::INFO) << "node is inited successfully";
  }

  log(Logging::INFO) << "Spawning p2p server";

  System::Event p2pStarted(*dispatcher);
  
  System::Context<> context(*dispatcher, [&]() {
    p2pStarted.set();
    p2pNode.run();
  });

  p2pStarted.wait();

  runWalletService(currency, *node);

  p2pNode.sendStopSignal();
  context.get();
  node->shutdown();
  p2pNode.deinit(); 
}

void PaymentGateService::runRpcProxy(Logging::LoggerRef& log) {
  log(Logging::INFO) << "Starting Payment Gate with remote node";
  VarNote::Currency currency = currencyBuilder.currency();
  
  std::unique_ptr<VarNote::INode> node(
    PaymentService::NodeFactory::createNode(
      config.remoteNodeConfig.daemonHost, 
      config.remoteNodeConfig.daemonPort));

  runWalletService(currency, *node);
}

void PaymentGateService::runWalletService(const VarNote::Currency& currency, VarNote::INode& node) {
  PaymentService::WalletConfiguration walletConfiguration{
    config.gateConfiguration.containerFile,
    config.gateConfiguration.containerPassword
  };

  std::unique_ptr<VarNote::WalletGreen> wallet(new VarNote::WalletGreen(*dispatcher, currency, node, logger));

  service = new PaymentService::WalletService(currency, *dispatcher, node, *wallet, *wallet, walletConfiguration, logger);
  std::unique_ptr<PaymentService::WalletService> serviceGuard(service);
  try {
    service->init();
  } catch (std::exception& e) {
    Logging::LoggerRef(logger, "run")(Logging::ERROR, Logging::BRIGHT_RED) << "Failed to init walletService reason: " << e.what();
    return;
  }

  if (config.gateConfiguration.printAddresses) {
    // print addresses and exit
    std::vector<std::string> addresses;
    service->getAddresses(addresses);
    for (const auto& address: addresses) {
      std::cout << "Address: " << address << std::endl;
    }
  } else {
    PaymentService::PaymentServiceJsonRpcServer rpcServer(*dispatcher, *stopEvent, *service, logger);
    rpcServer.start(config.gateConfiguration.bindAddress, config.gateConfiguration.bindPort);

    Logging::LoggerRef(logger, "PaymentGateService")(Logging::INFO, Logging::BRIGHT_WHITE) << "JSON-RPC server stopped, stopping wallet service...";

    try {
      service->saveWallet();
    } catch (std::exception& ex) {
      Logging::LoggerRef(logger, "saveWallet")(Logging::WARNING, Logging::YELLOW) << "Couldn't save container: " << ex.what();
    }
  }
}
