/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#pragma once

#include "../pch.hpp"

/// Function-level detours into gta_sa.exe: disable single-player menus,
/// suppress the stock loading screen, neuter built-in cheat handlers, etc.
/// Each detour is registered through MinHook; addresses live in Game/game_sa.hpp
/// and are version-gated to the supported gta_sa.exe build.
class GameHooks
{
public:
	GameHooks();
	~GameHooks();

	GameHooks(const GameHooks&) = delete;
	GameHooks& operator=(const GameHooks&) = delete;

	/// Registers every detour and enables them in one batch.
	bool install();

	/// Disables every detour and releases the MinHook entries.
	void uninstall();

	bool installed() const { return installed_; }

private:
	bool installed_ = false;
};
