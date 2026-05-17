/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#include "game_detect.hpp"

#include <filesystem>

namespace
{
	namespace fs = std::filesystem;

	std::optional<GtaInstall> validate(const std::wstring& dir)
	{
		if (dir.empty())
		{
			return std::nullopt;
		}
		fs::path exe = fs::path(dir) / L"gta_sa.exe";
		std::error_code ec;
		if (fs::exists(exe, ec))
		{
			return GtaInstall { dir, exe.wstring() };
		}
		return std::nullopt;
	}

	std::wstring readRegistry(HKEY root, const wchar_t* sub, const wchar_t* val)
	{
		HKEY h = nullptr;
		if (RegOpenKeyExW(root, sub, 0, KEY_READ | KEY_WOW64_32KEY, &h) != ERROR_SUCCESS)
		{
			return {};
		}
		wchar_t buf[MAX_PATH] = {};
		DWORD size = sizeof(buf);
		DWORD type = 0;
		const LSTATUS rc = RegQueryValueExW(h, val, nullptr, &type, reinterpret_cast<LPBYTE>(buf), &size);
		RegCloseKey(h);
		if (rc != ERROR_SUCCESS || (type != REG_SZ && type != REG_EXPAND_SZ))
		{
			return {};
		}
		return std::wstring(buf);
	}

	std::wstring readEnv(const wchar_t* name)
	{
		wchar_t buf[MAX_PATH] = {};
		const DWORD n = GetEnvironmentVariableW(name, buf, MAX_PATH);
		if (n == 0 || n >= MAX_PATH)
		{
			return {};
		}
		return std::wstring(buf, n);
	}
}

std::optional<GtaInstall> detectGta(const std::wstring& cliOverride)
{
	if (auto ok = validate(cliOverride))
	{
		return ok;
	}
	if (auto ok = validate(readEnv(L"OMP_GTA_DIR")))
	{
		return ok;
	}
	for (HKEY root : { HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER })
	{
		if (auto ok = validate(readRegistry(root, L"SOFTWARE\\Rockstar Games\\GTA San Andreas", L"InstallFolder")))
		{
			return ok;
		}
	}
	return std::nullopt;
}
