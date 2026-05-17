/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#pragma once

#include "config.hpp"
#include "pch.hpp"

class D3D9DeviceHook;
class WndProcHook;
class GameHooks;
class Overlay;
class NetClient;

/// Composition root of the client DLL. Owns every subsystem; constructed once
/// from the bootstrap thread spawned in DllMain.
class ClientCore
{
public:
	/// Singleton accessor. The DLL has exactly one ClientCore for its lifetime.
	static ClientCore& instance();

	/// Initialises subsystems and installs hooks. Idempotent.
	/// @param module The HMODULE handed to DllMain.
	/// @param config Configuration resolved at start-up.
	/// @returns true on success.
	bool initialize(HMODULE module, Config config);

	/// Tears subsystems down and reverts hooks. Called from DllMain on detach.
	void shutdown();

	const Config& config() const { return config_; }
	HMODULE module() const { return module_; }

	Overlay& overlay() { return *overlay_; }
	NetClient& net() { return *net_; }

private:
	ClientCore() = default;
	~ClientCore() = default;
	ClientCore(const ClientCore&) = delete;
	ClientCore& operator=(const ClientCore&) = delete;

	HMODULE module_ = nullptr;
	Config config_;
	std::atomic<bool> initialized_ { false };

	std::unique_ptr<D3D9DeviceHook> d3d9_;
	std::unique_ptr<WndProcHook> wndProc_;
	std::unique_ptr<GameHooks> gameHooks_;
	std::unique_ptr<Overlay> overlay_;
	std::unique_ptr<NetClient> net_;
};
