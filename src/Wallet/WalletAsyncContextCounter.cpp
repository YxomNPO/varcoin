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

#include "WalletAsyncContextCounter.h"

namespace VarNote {

void WalletAsyncContextCounter::addAsyncContext() {
  std::unique_lock<std::mutex> lock(m_mutex);
  m_asyncContexts++;
}

void WalletAsyncContextCounter::delAsyncContext() {
  std::unique_lock<std::mutex> lock(m_mutex);
  m_asyncContexts--;

  if (!m_asyncContexts) m_cv.notify_one();
}

void WalletAsyncContextCounter::waitAsyncContextsFinish() {
  std::unique_lock<std::mutex> lock(m_mutex);
  while (m_asyncContexts > 0)
    m_cv.wait(lock);
}

} //namespace VarNote
