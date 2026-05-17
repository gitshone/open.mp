/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#pragma once

#include "../pch.hpp"

/// Captures the IDirect3DDevice9 created by gta_sa.exe and inserts pre/post
/// callbacks around Present, EndScene and Reset. Implementation creates a
/// throwaway device at install() time to read the d3d9.dll vtable, then
/// detours the actual function pointers via MinHook — the game's device
/// shares the same vtable so its calls route through the same trampolines.
class D3D9DeviceHook
{
public:
	using FrameCallback = std::function<void(IDirect3DDevice9*, HWND)>;
	using DeviceCallback = std::function<void(IDirect3DDevice9*)>;

	D3D9DeviceHook();
	~D3D9DeviceHook();

	D3D9DeviceHook(const D3D9DeviceHook&) = delete;
	D3D9DeviceHook& operator=(const D3D9DeviceHook&) = delete;

	/// Reads the d3d9.dll vtable through a dummy device and arms the MinHook
	/// detours for Present/EndScene/Reset.
	/// @returns true on success.
	bool install();

	/// Disables and removes the MinHook detours.
	void uninstall();

	bool installed() const { return installed_; }

	/// Set on the very first EndScene call; null until then.
	IDirect3DDevice9* device() const { return device_; }
	HWND window() const { return window_; }

	/// Fired exactly once, when the game's device is observed for the first time.
	void onDeviceAcquired(FrameCallback cb) { onAcquired_ = std::move(cb); }

	/// Fired every frame from inside EndScene, after the game has issued its
	/// draw calls but before the trampoline returns.
	void onEndScene(DeviceCallback cb) { onEnd_ = std::move(cb); }

	/// Fired immediately before the original Reset runs.
	void onDeviceLost(DeviceCallback cb) { onLost_ = std::move(cb); }

	/// Fired immediately after a successful Reset.
	void onDeviceReset(DeviceCallback cb) { onReset_ = std::move(cb); }

private:
	/// Detour entry points; route through self_ to reach instance state.
	static HRESULT STDMETHODCALLTYPE present(IDirect3DDevice9* dev, const RECT* src, const RECT* dst, HWND wnd, const RGNDATA* dirty);
	static HRESULT STDMETHODCALLTYPE endScene(IDirect3DDevice9* dev);
	static HRESULT STDMETHODCALLTYPE reset(IDirect3DDevice9* dev, D3DPRESENT_PARAMETERS* pp);

	bool installed_ = false;
	IDirect3DDevice9* device_ = nullptr;
	HWND window_ = nullptr;

	FrameCallback onAcquired_;
	DeviceCallback onEnd_;
	DeviceCallback onLost_;
	DeviceCallback onReset_;

	static D3D9DeviceHook* self_;
};
