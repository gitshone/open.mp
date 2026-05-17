/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#include "game_hooks.hpp"

#include <MinHook.h>
#include <spdlog/spdlog.h>

GameHooks::GameHooks() = default;

GameHooks::~GameHooks()
{
	if (installed_)
	{
		uninstall();
	}
}

bool GameHooks::install()
{
	if (installed_)
	{
		return true;
	}
	// TODO: MH_CreateHook for each game function whose addresses are listed in
	// Game/game_sa.hpp; MH_EnableHook(MH_ALL_HOOKS) at the end of the batch.
	installed_ = true;
	spdlog::info("Game function hooks installed");
	return true;
}

void GameHooks::uninstall()
{
	installed_ = false;
}
