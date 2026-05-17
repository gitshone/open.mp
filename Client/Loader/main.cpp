/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#include "game_detect.hpp"
#include "inject.hpp"

#include <windows.h>
#include <shellapi.h>

#include <filesystem>
#include <string>

namespace
{
	struct Cli
	{
		std::wstring host;
		std::wstring port = L"7777";
		std::wstring nick;
		std::wstring pass;
		std::wstring gtaDir;
	};

	Cli parseArgv(int argc, wchar_t** argv)
	{
		Cli c;
		for (int i = 1; i + 1 < argc; ++i)
		{
			const std::wstring k = argv[i];
			const std::wstring v = argv[i + 1];
			if (k == L"--host" || k == L"-h")
			{
				c.host = v;
			}
			else if (k == L"--port" || k == L"-p")
			{
				c.port = v;
			}
			else if (k == L"--nick" || k == L"-n")
			{
				c.nick = v;
			}
			else if (k == L"--pass" || k == L"-z")
			{
				c.pass = v;
			}
			else if (k == L"--gta-dir")
			{
				c.gtaDir = v;
			}
			else
			{
				continue;
			}
			++i;
		}
		return c;
	}

	void exportEnv(const Cli& c)
	{
		if (!c.host.empty())
		{
			SetEnvironmentVariableW(L"OMP_HOST", c.host.c_str());
		}
		if (!c.port.empty())
		{
			SetEnvironmentVariableW(L"OMP_PORT", c.port.c_str());
		}
		if (!c.nick.empty())
		{
			SetEnvironmentVariableW(L"OMP_NICK", c.nick.c_str());
		}
		if (!c.pass.empty())
		{
			SetEnvironmentVariableW(L"OMP_PASS", c.pass.c_str());
		}
	}

	void showError(const std::wstring& msg)
	{
		MessageBoxW(nullptr, msg.c_str(), L"open.mp", MB_OK | MB_ICONERROR);
	}
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
	int argc = 0;
	wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	const Cli c = parseArgv(argc, argv);
	LocalFree(argv);

	const auto install = detectGta(c.gtaDir);
	if (!install)
	{
		showError(L"Could not locate gta_sa.exe.\nUse --gta-dir, set OMP_GTA_DIR, or install GTA: San Andreas (1.0 US).");
		return 1;
	}

	wchar_t selfPath[MAX_PATH] = {};
	GetModuleFileNameW(nullptr, selfPath, MAX_PATH);
	const std::wstring clientDll = (std::filesystem::path(selfPath).parent_path() / L"openmp-client.dll").wstring();
	if (!std::filesystem::exists(clientDll))
	{
		showError(L"openmp-client.dll not found next to the loader.");
		return 2;
	}

	exportEnv(c);

	InjectOptions opt;
	opt.gameExe = install->executablePath;
	opt.gameWorkDir = install->directory;
	opt.clientDll = clientDll;

	const auto result = spawnAndInject(opt);
	if (!result.ok)
	{
		showError(L"Failed to launch GTA: " + result.message);
		return 3;
	}
	return 0;
}
