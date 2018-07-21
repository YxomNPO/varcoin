// Copyright (c) 2012-2018, The CryptoNote developers, YxomTech.// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#pragma once

// defines are for Windows resource compiler
#define varcoin_VERSION_WINDOWS_COMMA 03, 18, 7, 22
#define varcoin_VERSION_STRING "0.3.0"

#ifndef RC_INVOKED  // Windows resource compiler

namespace varcoin {
inline const char *app_version() { return varcoin_VERSION_STRING; }
}

#endif
