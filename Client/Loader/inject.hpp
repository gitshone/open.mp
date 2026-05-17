/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#pragma once

#include <windows.h>

#include <string>

/// Inputs to spawnAndInject.
struct InjectOptions
{
	std::wstring gameExe;
	std::wstring gameWorkDir;
	std::wstring clientDll;
	std::wstring commandLine;
};

/// Result of an injection attempt.
struct InjectResult
{
	bool ok = false;
	DWORD lastError = 0;
	std::wstring message;
};

/// Spawns gta_sa.exe suspended, allocates a path buffer in its address space,
/// LoadLibraryW's the client DLL via CreateRemoteThread, then resumes the main
/// thread. No reliance on legacy samp.dll for any step.
InjectResult spawnAndInject(const InjectOptions& opt);
