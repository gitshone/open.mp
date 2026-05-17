/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#pragma once

#include "../pch.hpp"

/// Subclasses GTA SA's main window so input can be routed to the overlay when
/// chat, dialogs or cursor mode are active and passed through to the game
/// otherwise. Messages are forwarded to ImGui_ImplWin32_WndProcHandler before
/// the overlay handler runs. The actual subclass is deferred until attach()
/// because the window doesn't exist at DLL load time.
class WndProcHook
{
public:
	using MessageHandler = std::function<bool(HWND, UINT, WPARAM, LPARAM)>;

	WndProcHook();
	~WndProcHook();

	WndProcHook(const WndProcHook&) = delete;
	WndProcHook& operator=(const WndProcHook&) = delete;

	/// Prepares the hook. No-op without an HWND; the subclass is installed
	/// from attach() once D3D9 resolves the focus window.
	bool install();

	/// Reverts the subclass if attached and clears state.
	void uninstall();

	/// Installs the subclass on the given window. Idempotent across HWNDs;
	/// re-attaching to a different window detaches from the previous one.
	bool attach(HWND window);

	bool installed() const { return installed_; }
	HWND window() const { return window_; }

	/// Optional callback for messages reaching the window. Returning true
	/// suppresses propagation to the game's WndProc.
	void setOverlayHandler(MessageHandler h) { overlayHandler_ = std::move(h); }

private:
	static LRESULT CALLBACK trampoline(HWND, UINT, WPARAM, LPARAM);

	bool installed_ = false;
	HWND window_ = nullptr;
	WNDPROC originalProc_ = nullptr;
	MessageHandler overlayHandler_;

	static WndProcHook* self_;
};
