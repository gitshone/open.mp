/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#include "client_core.hpp"

#include "Game/game_sa.hpp"
#include "Hooks/d3d9_device_hook.hpp"
#include "Hooks/game_hooks.hpp"
#include "Hooks/wnd_proc_hook.hpp"
#include "Network/net_client.hpp"
#include "Render/overlay.hpp"

#include <MinHook.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

ClientCore& ClientCore::instance()
{
	static ClientCore inst;
	return inst;
}

bool ClientCore::initialize(HMODULE module, Config config)
{
	if (initialized_.exchange(true))
	{
		return true;
	}
	module_ = module;
	config_ = std::move(config);

	spdlog::set_default_logger(spdlog::basic_logger_mt("openmp", config_.logFile, true));
	spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
	spdlog::info("open.mp client {}.{}.{} starting",
		OMP_CLIENT_VERSION_MAJOR, OMP_CLIENT_VERSION_MINOR, OMP_CLIENT_VERSION_PATCH);

	if (!GameSA::detect())
	{
		spdlog::error("gta_sa.exe signature mismatch; refusing to attach");
		return false;
	}

	if (MH_Initialize() != MH_OK)
	{
		spdlog::error("MinHook initialisation failed");
		return false;
	}

	gameHooks_ = std::make_unique<GameHooks>();
	wndProc_ = std::make_unique<WndProcHook>();
	d3d9_ = std::make_unique<D3D9DeviceHook>();
	overlay_ = std::make_unique<Overlay>();
	net_ = std::make_unique<NetClient>();

	d3d9_->onDeviceAcquired([this](IDirect3DDevice9* dev, HWND hwnd) {
		overlay_->onDeviceAcquired(dev, hwnd);
		wndProc_->attach(hwnd);
	});
	d3d9_->onEndScene([this](IDirect3DDevice9*) { overlay_->render(); });
	d3d9_->onDeviceLost([this](IDirect3DDevice9*) { overlay_->onDeviceLost(); });
	d3d9_->onDeviceReset([this](IDirect3DDevice9*) { overlay_->onDeviceReset(); });

	// Function-level patches go in before the window/device proxies so the very
	// first frame already has our overlay surface available.
	gameHooks_->install();
	wndProc_->install();
	d3d9_->install();

	return true;
}

void ClientCore::shutdown()
{
	if (!initialized_.exchange(false))
	{
		return;
	}
	spdlog::info("open.mp client shutting down");

	if (net_)
	{
		net_->disconnect();
	}
	if (d3d9_)
	{
		d3d9_->uninstall();
	}
	if (wndProc_)
	{
		wndProc_->uninstall();
	}
	if (gameHooks_)
	{
		gameHooks_->uninstall();
	}

	MH_Uninitialize();

	net_.reset();
	overlay_.reset();
	d3d9_.reset();
	wndProc_.reset();
	gameHooks_.reset();

	spdlog::shutdown();
}
