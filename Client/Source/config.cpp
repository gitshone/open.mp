/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#include "config.hpp"

#include <toml++/toml.hpp>

#include <cstdlib>
#include <filesystem>

namespace
{
	/// Reads an env var into a string. Returns the fallback when the variable
	/// is unset or empty.
	std::string envOr(const char* key, std::string fallback = {})
	{
		char* buf = nullptr;
		std::size_t len = 0;
		if (_dupenv_s(&buf, &len, key) == 0 && buf)
		{
			std::string out(buf);
			std::free(buf);
			if (!out.empty())
			{
				return out;
			}
		}
		return fallback;
	}

	/// Locates openmp-client.toml beside the loaded DLL.
	std::filesystem::path tomlPath()
	{
		wchar_t path[MAX_PATH] = {};
		HMODULE self = nullptr;
		GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			reinterpret_cast<LPCWSTR>(&tomlPath), &self);
		GetModuleFileNameW(self, path, MAX_PATH);
		return std::filesystem::path(path).parent_path() / L"openmp-client.toml";
	}
}

Config Config::load()
{
	Config c;

	const auto path = tomlPath();
	if (std::filesystem::exists(path))
	{
		const auto tbl = toml::parse_file(path.string());
		c.serverHost = tbl["server"]["host"].value_or<std::string>("");
		c.serverPort = static_cast<std::uint16_t>(tbl["server"]["port"].value_or(7777));
		c.nickname = tbl["player"]["nickname"].value_or<std::string>("");
		c.password = tbl["server"]["password"].value_or<std::string>("");
		c.rconPassword = tbl["server"]["rcon_password"].value_or<std::string>("");
		c.loadingScreenEnabled = tbl["loading_screen"]["enabled"].value_or(true);
		c.loadingScreenImage = tbl["loading_screen"]["image"].value_or<std::string>("");
		c.logFile = tbl["logging"]["file"].value_or<std::string>("openmp-client.log");
	}

	c.serverHost = envOr("OMP_HOST", c.serverHost);
	if (const auto port = envOr("OMP_PORT"); !port.empty())
	{
		c.serverPort = static_cast<std::uint16_t>(std::atoi(port.c_str()));
	}
	c.nickname = envOr("OMP_NICK", c.nickname);
	c.password = envOr("OMP_PASS", c.password);
	c.rconPassword = envOr("OMP_RCON", c.rconPassword);
	c.loadingScreenImage = envOr("OMP_LOADSCREEN", c.loadingScreenImage);

	return c;
}
