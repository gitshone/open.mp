/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#pragma once

#include <windows.h>

#include <optional>
#include <string>

/// A resolved GTA: San Andreas installation.
struct GtaInstall
{
	std::wstring directory;
	std::wstring executablePath;
};

/// Searches, in priority order:
///   1. The CLI override passed from main.cpp.
///   2. The OMP_GTA_DIR environment variable.
///   3. HKLM / HKCU\Software\Rockstar Games\GTA San Andreas\InstallFolder.
/// Returns nullopt when none yields a readable gta_sa.exe.
std::optional<GtaInstall> detectGta(const std::wstring& cliOverride = {});
