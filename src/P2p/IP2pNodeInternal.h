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

#include <list>
#include "P2pProtocolDefinitions.h"

namespace VarNote {

class P2pContext;

class IP2pNodeInternal {
public:
  virtual const CORE_SYNC_DATA& getGenesisPayload() const = 0;
  virtual std::list<PeerlistEntry> getLocalPeerList() const = 0;
  virtual basic_node_data getNodeData() const = 0;
  virtual PeerIdType getPeerId() const = 0;

  virtual void handleNodeData(const basic_node_data& node, P2pContext& ctx) = 0;
  virtual bool handleRemotePeerList(const std::list<PeerlistEntry>& peerlist, time_t local_time) = 0;
  virtual void tryPing(P2pContext& ctx) = 0;
};

}
