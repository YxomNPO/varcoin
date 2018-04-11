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

#include "WalletUtils.h"

#include "VarNote.h"
#include "crypto/crypto.h"
#include "Wallet/WalletErrors.h"

namespace VarNote {

void throwIfKeysMismatch(const Crypto::SecretKey& secretKey, const Crypto::PublicKey& expectedPublicKey, const std::string& message) {
  Crypto::PublicKey pub;
  bool r = Crypto::secret_key_to_public_key(secretKey, pub);
  if (!r || expectedPublicKey != pub) {
    throw std::system_error(make_error_code(VarNote::error::WRONG_PASSWORD), message);
  }
}

bool validateAddress(const std::string& address, const VarNote::Currency& currency) {
  VarNote::AccountPublicAddress ignore;
  return currency.parseAccountAddressString(address, ignore);
}

std::ostream& operator<<(std::ostream& os, VarNote::WalletTransactionState state) {
  switch (state) {
  case VarNote::WalletTransactionState::SUCCEEDED:
    os << "SUCCEEDED";
    break;
  case VarNote::WalletTransactionState::FAILED:
    os << "FAILED";
    break;
  case VarNote::WalletTransactionState::CANCELLED:
    os << "CANCELLED";
    break;
  case VarNote::WalletTransactionState::CREATED:
    os << "CREATED";
    break;
  case VarNote::WalletTransactionState::DELETED:
    os << "DELETED";
    break;
  default:
    os << "<UNKNOWN>";
  }

  return os << " (" << static_cast<int>(state) << ')';
}

std::ostream& operator<<(std::ostream& os, VarNote::WalletTransferType type) {
  switch (type) {
  case VarNote::WalletTransferType::USUAL:
    os << "USUAL";
    break;
  case VarNote::WalletTransferType::DONATION:
    os << "DONATION";
    break;
  case VarNote::WalletTransferType::CHANGE:
    os << "CHANGE";
    break;
  default:
    os << "<UNKNOWN>";
  }

  return os << " (" << static_cast<int>(type) << ')';
}

std::ostream& operator<<(std::ostream& os, VarNote::WalletGreen::WalletState state) {
  switch (state) {
  case VarNote::WalletGreen::WalletState::INITIALIZED:
    os << "INITIALIZED";
    break;
  case VarNote::WalletGreen::WalletState::NOT_INITIALIZED:
    os << "NOT_INITIALIZED";
    break;
  default:
    os << "<UNKNOWN>";
  }

  return os << " (" << static_cast<int>(state) << ')';
}

std::ostream& operator<<(std::ostream& os, VarNote::WalletGreen::WalletTrackingMode mode) {
  switch (mode) {
  case VarNote::WalletGreen::WalletTrackingMode::TRACKING:
    os << "TRACKING";
    break;
  case VarNote::WalletGreen::WalletTrackingMode::NOT_TRACKING:
    os << "NOT_TRACKING";
    break;
  case VarNote::WalletGreen::WalletTrackingMode::NO_ADDRESSES:
    os << "NO_ADDRESSES";
    break;
  default:
    os << "<UNKNOWN>";
  }

  return os << " (" << static_cast<int>(mode) << ')';
}

TransferListFormatter::TransferListFormatter(const VarNote::Currency& currency, const WalletGreen::TransfersRange& range) :
  m_currency(currency),
  m_range(range) {
}

void TransferListFormatter::print(std::ostream& os) const {
  for (auto it = m_range.first; it != m_range.second; ++it) {
    os << '\n' << std::setw(21) << m_currency.formatAmount(it->second.amount) <<
      ' ' << (it->second.address.empty() ? "<UNKNOWN>" : it->second.address) <<
      ' ' << it->second.type;
  }
}

std::ostream& operator<<(std::ostream& os, const TransferListFormatter& formatter) {
  formatter.print(os);
  return os;
}

WalletOrderListFormatter::WalletOrderListFormatter(const VarNote::Currency& currency, const std::vector<VarNote::WalletOrder>& walletOrderList) :
  m_currency(currency),
  m_walletOrderList(walletOrderList) {
}

void WalletOrderListFormatter::print(std::ostream& os) const {
  os << '{';

  if (!m_walletOrderList.empty()) {
    os << '<' << m_currency.formatAmount(m_walletOrderList.front().amount) << ", " << m_walletOrderList.front().address << '>';

    for (auto it = std::next(m_walletOrderList.begin()); it != m_walletOrderList.end(); ++it) {
      os << '<' << m_currency.formatAmount(it->amount) << ", " << it->address << '>';
    }
  }

  os << '}';
}

std::ostream& operator<<(std::ostream& os, const WalletOrderListFormatter& formatter) {
  formatter.print(os);
  return os;
}

}
