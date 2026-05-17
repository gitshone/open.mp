/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#include "game_sa.hpp"

bool GameSA::detect()
{
	// TODO: validate checksum / known version marker before any patch is applied.
	return GetModuleHandleW(L"gta_sa.exe") != nullptr;
}

std::uintptr_t GameSA::base()
{
	return reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"gta_sa.exe"));
}

HWND GameSA::mainWindow()
{
	return FindWindowW(L"Grand theft auto San Andreas", nullptr);
}
