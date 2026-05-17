/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#pragma once

#include "../pch.hpp"

/// Clean-room accessors into gta_sa.exe. Addresses must come from public
/// modding references (GTAModding wiki, OpenIV, etc.); the SA-MP
/// decompilation under SA-MP/ is NOT an acceptable upstream.
class GameSA
{
public:
	/// Verifies the loaded gta_sa.exe matches the supported 1.0 US build.
	static bool detect();

	/// Module base of gta_sa.exe.
	static std::uintptr_t base();

	/// Main rendering window, or NULL while the game is still starting up.
	static HWND mainWindow();
};
