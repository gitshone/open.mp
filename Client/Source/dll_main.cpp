/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#include "client_core.hpp"
#include "pch.hpp"

namespace
{
	/// Bootstrap thread. DllMain runs under loader lock so anything beyond the
	/// trivial must happen on a worker thread that exits before we attempt
	/// any D3D work.
	DWORD WINAPI bootstrap(LPVOID param)
	{
		auto* module = static_cast<HMODULE>(param);
		ClientCore::instance().initialize(module, Config::load());
		return 0;
	}
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID /*reserved*/)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(module);
		CreateThread(nullptr, 0, bootstrap, module, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		ClientCore::instance().shutdown();
		break;
	}
	return TRUE;
}
