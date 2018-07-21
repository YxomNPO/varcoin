// Copyright (c) 2012-2018, The CryptoNote developers, YxomTech.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#include "PathTools.hpp"
#include <algorithm>
#include <cstdio>
#include "Files.hpp"

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#ifdef _WIN32
#include <shlobj.h>
#include <strsafe.h>
#include "platform/Windows.hpp"
#pragma warning(disable : 4996)  // Deprecated GetVersionA
#else
#include <sys/stat.h>
#include <sys/utsname.h>
#include <boost/algorithm/string/trim.hpp>
#endif

#ifdef __ANDROID__
#include <QStandardPaths>
#endif

namespace platform {
#ifdef _WIN32
std::string get_os_version_string() {
	typedef void(WINAPI * PGNSI)(LPSYSTEM_INFO);
	typedef BOOL(WINAPI * PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);
#define BUFSIZE 10000

	char pszOS[BUFSIZE] = {0};
	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;
	PGNSI pGNSI;
	PGPI pGPI;
	BOOL bOsVersionInfoEx;
	DWORD dwType;

	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	bOsVersionInfoEx         = GetVersionExA((OSVERSIONINFO *)&osvi);

	if (!bOsVersionInfoEx)
		return pszOS;

	// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.

	pGNSI = (PGNSI)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
	if (NULL != pGNSI)
		pGNSI(&si);
	else
		GetSystemInfo(&si);

	if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion > 4) {
		StringCchCopy(pszOS, BUFSIZE, TEXT("Microsoft "));

		// Test for the specific product.

		if (osvi.dwMajorVersion == 6) {
			if (osvi.dwMinorVersion == 0) {
				if (osvi.wProductType == VER_NT_WORKSTATION)
					StringCchCat(pszOS, BUFSIZE, TEXT("Windows Vista "));
				else
					StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2008 "));
			}

			if (osvi.dwMinorVersion == 1) {
				if (osvi.wProductType == VER_NT_WORKSTATION)
					StringCchCat(pszOS, BUFSIZE, TEXT("Windows 7 "));
				else
					StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2008 R2 "));
			}

			pGPI = (PGPI)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo");

			pGPI(osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);

			switch (dwType) {
			case PRODUCT_ULTIMATE:
				StringCchCat(pszOS, BUFSIZE, TEXT("Ultimate Edition"));
				break;
			case PRODUCT_PROFESSIONAL:
				StringCchCat(pszOS, BUFSIZE, TEXT("Professional"));
				break;
			case PRODUCT_HOME_PREMIUM:
				StringCchCat(pszOS, BUFSIZE, TEXT("Home Premium Edition"));
				break;
			case PRODUCT_HOME_BASIC:
				StringCchCat(pszOS, BUFSIZE, TEXT("Home Basic Edition"));
				break;
			case PRODUCT_ENTERPRISE:
				StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition"));
				break;
			case PRODUCT_BUSINESS:
				StringCchCat(pszOS, BUFSIZE, TEXT("Business Edition"));
				break;
			case PRODUCT_STARTER:
				StringCchCat(pszOS, BUFSIZE, TEXT("Starter Edition"));
				break;
			case PRODUCT_CLUSTER_SERVER:
				StringCchCat(pszOS, BUFSIZE, TEXT("Cluster Server Edition"));
				break;
			case PRODUCT_DATACENTER_SERVER:
				StringCchCat(pszOS, BUFSIZE, TEXT("Datacenter Edition"));
				break;
			case PRODUCT_DATACENTER_SERVER_CORE:
				StringCchCat(pszOS, BUFSIZE, TEXT("Datacenter Edition (core installation)"));
				break;
			case PRODUCT_ENTERPRISE_SERVER:
				StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition"));
				break;
			case PRODUCT_ENTERPRISE_SERVER_CORE:
				StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition (core installation)"));
				break;
			case PRODUCT_ENTERPRISE_SERVER_IA64:
				StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition for Itanium-based Systems"));
				break;
			case PRODUCT_SMALLBUSINESS_SERVER:
				StringCchCat(pszOS, BUFSIZE, TEXT("Small Business Server"));
				break;
			case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
				StringCchCat(pszOS, BUFSIZE, TEXT("Small Business Server Premium Edition"));
				break;
			case PRODUCT_STANDARD_SERVER:
				StringCchCat(pszOS, BUFSIZE, TEXT("Standard Edition"));
				break;
			case PRODUCT_STANDARD_SERVER_CORE:
				StringCchCat(pszOS, BUFSIZE, TEXT("Standard Edition (core installation)"));
				break;
			case PRODUCT_WEB_SERVER:
				StringCchCat(pszOS, BUFSIZE, TEXT("Web Server Edition"));
				break;
			}
		}

		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2) {
			if (GetSystemMetrics(SM_SERVERR2))
				StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2003 R2, "));
			else if (osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER)
				StringCchCat(pszOS, BUFSIZE, TEXT("Windows Storage Server 2003"));
			else if (osvi.wSuiteMask & VER_SUITE_WH_SERVER)
				StringCchCat(pszOS, BUFSIZE, TEXT("Windows Home Server"));
			else if (osvi.wProductType == VER_NT_WORKSTATION &&
			         si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
				StringCchCat(pszOS, BUFSIZE, TEXT("Windows XP Professional x64 Edition"));
			} else
				StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2003, "));

			// Test for the server type.
			if (osvi.wProductType != VER_NT_WORKSTATION) {
				if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) {
					if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
						StringCchCat(pszOS, BUFSIZE, TEXT("Datacenter Edition for Itanium-based Systems"));
					else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
						StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition for Itanium-based Systems"));
				}

				else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
					if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
						StringCchCat(pszOS, BUFSIZE, TEXT("Datacenter x64 Edition"));
					else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
						StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise x64 Edition"));
					else
						StringCchCat(pszOS, BUFSIZE, TEXT("Standard x64 Edition"));
				}

				else {
					if (osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
						StringCchCat(pszOS, BUFSIZE, TEXT("Compute Cluster Edition"));
					else if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
						StringCchCat(pszOS, BUFSIZE, TEXT("Datacenter Edition"));
					else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
						StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition"));
					else if (osvi.wSuiteMask & VER_SUITE_BLADE)
						StringCchCat(pszOS, BUFSIZE, TEXT("Web Edition"));
					else
						StringCchCat(pszOS, BUFSIZE, TEXT("Standard Edition"));
				}
			}
		}

		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) {
			StringCchCat(pszOS, BUFSIZE, TEXT("Windows XP "));
			if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
				StringCchCat(pszOS, BUFSIZE, TEXT("Home Edition"));
			else
				StringCchCat(pszOS, BUFSIZE, TEXT("Professional"));
		}

		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0) {
			StringCchCat(pszOS, BUFSIZE, TEXT("Windows 2000 "));

			if (osvi.wProductType == VER_NT_WORKSTATION) {
				StringCchCat(pszOS, BUFSIZE, TEXT("Professional"));
			} else {
				if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
					StringCchCat(pszOS, BUFSIZE, TEXT("Datacenter Server"));
				else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
					StringCchCat(pszOS, BUFSIZE, TEXT("Advanced Server"));
				else
					StringCchCat(pszOS, BUFSIZE, TEXT("Server"));
			}
		}

		// Include service pack (if any) and build number.

		if (strlen(osvi.szCSDVersion) > 0) {
			StringCchCat(pszOS, BUFSIZE, TEXT(" "));
			StringCchCat(pszOS, BUFSIZE, osvi.szCSDVersion);
		}

		TCHAR buf[80];

		StringCchPrintf(buf, 80, TEXT(" (build %d)"), osvi.dwBuildNumber);
		StringCchCat(pszOS, BUFSIZE, buf);

		if (osvi.dwMajorVersion >= 6) {
			if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
				StringCchCat(pszOS, BUFSIZE, TEXT(", 64-bit"));
			else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
				StringCchCat(pszOS, BUFSIZE, TEXT(", 32-bit"));
		}

		return pszOS;
	} else {
		printf("This sample does not support this version of Windows.\n");
		return pszOS;
	}
}
#else
#if !TARGET_OS_IPHONE
std::string get_os_version_string() {
	utsname un;

	if (uname(&un) < 0)
		return std::string("*nix: failed to get os version");
	return std::string() + un.sysname + " " + un.version + " " + un.release;
}
#endif  // #if !TARGET_OS_IPHONE
#endif

#ifdef _WIN32
static std::string get_special_folder_path(int nfolder, bool iscreate) {
	wchar_t psz_path[MAX_PATH]{};
	if (SHGetSpecialFolderPathW(NULL, psz_path, nfolder, iscreate)) {
		return FileStream::utf16_to_utf8(psz_path);
	}
	return std::string();
}
#endif

#if !TARGET_OS_IPHONE
std::string get_default_data_directory(const std::string &varnote_name) {
#ifdef _WIN32
	return get_special_folder_path(CSIDL_APPDATA, true) + "/" + varnote_name;
#else
	std::string path_ret;
	const char *psz_home = getenv("HOME");
	if (psz_home)
		path_ret = psz_home;
	// Unix, including MAC_OSX
	return path_ret + "/." + varnote_name;
#endif
}
#endif  // #if !TARGET_OS_IPHONE

#ifdef __ANDROID__
std::string get_app_data_folder(const std::string &app_name) {
	QString data_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	return data_path.toStdString();
}
#elif !TARGET_OS_IPHONE
std::string get_app_data_folder(const std::string &app_name) {
#ifdef _WIN32
	// Windows
	return get_special_folder_path(CSIDL_APPDATA, true) + "\\" + app_name;
#else
	std::string path_ret;
	const char *psz_home = getenv("HOME");
	if (psz_home)
		path_ret = psz_home;
#if defined(__MACH__)
	return path_ret + "/Library/Application Support/" + app_name;
#endif
	return path_ret + "/." + app_name;
#endif
}
#endif  // #if !TARGET_OS_IPHONE

bool directory_exists(const std::string &path) {
#if defined(__MACH__) || defined(__linux__)
	struct stat info;

	if (stat(path.c_str(), &info) != 0)
		return false;  // printf( "cannot access %s\n", pathname );
	if (info.st_mode & S_IFDIR)
		return true;
	return false;
#elif defined(_WIN32)
	auto wsubpath  = FileStream::utf8_to_utf16(path);
	DWORD dwAttrib = GetFileAttributesW(wsubpath.c_str());

	return dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
#endif
}

bool create_directory_if_necessary(const std::string &subpath) {
#if defined(__MACH__) || defined(__linux__)
	mode_t mode = 0755;
	if (mkdir(subpath.c_str(), mode) != 0 && errno != EEXIST)
		return false;
	return true;
#elif defined(_WIN32)
	auto wsubpath = FileStream::utf8_to_utf16(subpath);
	DWORD last    = 0;
	if (CreateDirectoryW(wsubpath.c_str(), nullptr) == 0 && (last = GetLastError()) != ERROR_ALREADY_EXISTS)
		return false;
	return true;
#endif
}

std::string get_filename_without_directory(const std::string &path){
	size_t delim1 = path.rfind("/");
	size_t delim2 = path.rfind("\\");
	size_t delim_pos = delim1 == std::string::npos ? delim2 : delim2 == std::string::npos ? delim1 : std::max(delim1, delim2);
	return delim_pos != std::string::npos ? path.substr(delim_pos + 1) : path;
}

bool create_directories_if_necessary(const std::string &path) {
	size_t delim_pos = std::min(path.find("/"), path.find("\\"));
	while (delim_pos != std::string::npos) {
		create_directory_if_necessary(
		    path.substr(0, delim_pos + 1));  // We ignore intermediate results, because of some systems
		delim_pos = std::min(path.find("/", delim_pos + 1), path.find("\\", delim_pos + 1));
	}
	return create_directory_if_necessary(path);
}

bool atomic_replace_file(const std::string &replacement_name, const std::string &old_file_name) {
#if defined(_WIN32)
	// Maximizing chances for success
	DWORD attributes = ::GetFileAttributes(old_file_name.c_str());
	if (INVALID_FILE_ATTRIBUTES != attributes) {
		::SetFileAttributes(old_file_name.c_str(), attributes & (~FILE_ATTRIBUTE_READONLY));
	}
	auto wreplacement_name = FileStream::utf8_to_utf16(replacement_name);
	auto wold_file_name    = FileStream::utf8_to_utf16(old_file_name);

	bool ok = 0 != ::MoveFileExW(wreplacement_name.c_str(), wold_file_name.c_str(), MOVEFILE_REPLACE_EXISTING);
// int code = ok ? 0 : static_cast<int>(::GetLastError());
#else
	bool ok = 0 == std::rename(replacement_name.c_str(), old_file_name.c_str());
// int code = ok ? 0 : errno;
#endif
	// if(err) *err = std::error_code(code, std::system_category());
	return ok;
}
bool copy_file(const std::string &to_path, const std::string &from_path) {
	{  // TODO - optimize
		platform::FileStream from(from_path, platform::FileStream::READ_EXISTING);
		platform::FileStream to(to_path, platform::FileStream::TRUNCATE_READ_WRITE);
		auto si = from.seek(0, SEEK_END);
		from.seek(0, SEEK_SET);
		common::BinaryArray data(si);
		from.read(data.data(), data.size());
		to.write(data.data(), data.size());
	}
	return true;
}
}
