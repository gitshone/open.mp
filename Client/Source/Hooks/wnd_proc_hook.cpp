/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#include "wnd_proc_hook.hpp"

#include <spdlog/spdlog.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

WndProcHook* WndProcHook::self_ = nullptr;

WndProcHook::WndProcHook() = default;

WndProcHook::~WndProcHook()
{
	if (installed_)
	{
		uninstall();
	}
}

bool WndProcHook::install()
{
	if (installed_)
	{
		return true;
	}
	self_ = this;
	installed_ = true;
	return true;
}

void WndProcHook::uninstall()
{
	if (!installed_)
	{
		return;
	}
	if (window_ && originalProc_)
	{
		SetWindowLongPtrW(window_, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(originalProc_));
	}
	originalProc_ = nullptr;
	window_ = nullptr;
	self_ = nullptr;
	installed_ = false;
}

bool WndProcHook::attach(HWND window)
{
	if (!window)
	{
		return false;
	}
	if (window_ == window)
	{
		return true;
	}
	if (window_ && originalProc_)
	{
		SetWindowLongPtrW(window_, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(originalProc_));
		originalProc_ = nullptr;
	}
	window_ = window;
	originalProc_ = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(window_, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&trampoline)));
	if (!originalProc_)
	{
		spdlog::error("SetWindowLongPtrW(GWLP_WNDPROC) failed: 0x{:08x}", static_cast<unsigned>(GetLastError()));
		window_ = nullptr;
		return false;
	}
	spdlog::info("WndProc subclass attached to HWND 0x{:p}", static_cast<void*>(window_));
	return true;
}

LRESULT CALLBACK WndProcHook::trampoline(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wp, lp))
	{
		return 0;
	}
	if (self_ && self_->overlayHandler_ && self_->overlayHandler_(hwnd, msg, wp, lp))
	{
		return 0;
	}
	if (self_ && self_->originalProc_)
	{
		return CallWindowProcW(self_->originalProc_, hwnd, msg, wp, lp);
	}
	return DefWindowProcW(hwnd, msg, wp, lp);
}
