/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#pragma once

#include "pch.hpp"

/// Launch-time configuration handed to the client by the loader.
///
/// Resolved in priority order:
///   1. openmp-client.toml next to the DLL (operator-controlled defaults).
///   2. OMP_* environment variables set by the loader at spawn time.
class Config
{
public:
	std::string serverHost;
	std::uint16_t serverPort = 7777;
	std::string nickname;
	std::string password;
	std::string rconPassword;

	bool loadingScreenEnabled = true;
	std::string loadingScreenImage;

	std::string logFile = "openmp-client.log";

	/// Loads the toml file (if present) and overlays environment variables.
	/// Never throws; missing fields keep their defaults.
	static Config load();
};
