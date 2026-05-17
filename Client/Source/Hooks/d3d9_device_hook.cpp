/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#include "d3d9_device_hook.hpp"

#include <MinHook.h>
#include <spdlog/spdlog.h>

namespace
{
	/// Public IDirect3DDevice9 vtable indices from d3d9.h (DirectX 9 SDK).
	constexpr std::size_t kVtblReset = 16;
	constexpr std::size_t kVtblPresent = 17;
	constexpr std::size_t kVtblEndScene = 42;

	using PresentFn = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
	using EndSceneFn = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*);
	using ResetFn = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

	PresentFn oPresent = nullptr;
	EndSceneFn oEndScene = nullptr;
	ResetFn oReset = nullptr;
}

D3D9DeviceHook* D3D9DeviceHook::self_ = nullptr;

D3D9DeviceHook::D3D9DeviceHook() = default;

D3D9DeviceHook::~D3D9DeviceHook()
{
	if (installed_)
	{
		uninstall();
	}
}

bool D3D9DeviceHook::install()
{
	if (installed_)
	{
		return true;
	}

	IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d)
	{
		spdlog::error("Direct3DCreate9 returned null; cannot capture device");
		return false;
	}

	HWND tmp = GetDesktopWindow();
	D3DPRESENT_PARAMETERS pp = {};
	pp.Windowed = TRUE;
	pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pp.BackBufferFormat = D3DFMT_UNKNOWN;
	pp.hDeviceWindow = tmp;

	IDirect3DDevice9* dummy = nullptr;
	HRESULT hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, tmp,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &dummy);
	if (FAILED(hr) || !dummy)
	{
		spdlog::error("dummy CreateDevice failed: 0x{:08x}", static_cast<unsigned>(hr));
		d3d->Release();
		return false;
	}

	void** vtbl = *reinterpret_cast<void***>(dummy);
	void* presentAddr = vtbl[kVtblPresent];
	void* endSceneAddr = vtbl[kVtblEndScene];
	void* resetAddr = vtbl[kVtblReset];

	dummy->Release();
	d3d->Release();

	if (MH_CreateHook(presentAddr, reinterpret_cast<void*>(&present), reinterpret_cast<void**>(&oPresent)) != MH_OK
		|| MH_CreateHook(endSceneAddr, reinterpret_cast<void*>(&endScene), reinterpret_cast<void**>(&oEndScene)) != MH_OK
		|| MH_CreateHook(resetAddr, reinterpret_cast<void*>(&reset), reinterpret_cast<void**>(&oReset)) != MH_OK)
	{
		spdlog::error("MH_CreateHook failed for one or more D3D9 entries");
		MH_RemoveHook(MH_ALL_HOOKS);
		return false;
	}

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
	{
		spdlog::error("MH_EnableHook failed");
		MH_RemoveHook(MH_ALL_HOOKS);
		return false;
	}

	self_ = this;
	installed_ = true;
	spdlog::info("D3D9 device hook installed");
	return true;
}

void D3D9DeviceHook::uninstall()
{
	if (!installed_)
	{
		return;
	}
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);
	self_ = nullptr;
	device_ = nullptr;
	window_ = nullptr;
	installed_ = false;
}

HRESULT STDMETHODCALLTYPE D3D9DeviceHook::endScene(IDirect3DDevice9* dev)
{
	D3D9DeviceHook* h = self_;
	if (h && !h->device_)
	{
		// First frame; capture the device and resolve the focus window from
		// the creation parameters so the overlay backend can be initialised.
		D3DDEVICE_CREATION_PARAMETERS params = {};
		dev->GetCreationParameters(&params);
		h->device_ = dev;
		h->window_ = params.hFocusWindow;
		if (h->onAcquired_)
		{
			h->onAcquired_(dev, h->window_);
		}
	}
	if (h && h->onEnd_)
	{
		h->onEnd_(dev);
	}
	return oEndScene(dev);
}

HRESULT STDMETHODCALLTYPE D3D9DeviceHook::present(IDirect3DDevice9* dev, const RECT* src, const RECT* dst, HWND wnd, const RGNDATA* dirty)
{
	return oPresent(dev, src, dst, wnd, dirty);
}

HRESULT STDMETHODCALLTYPE D3D9DeviceHook::reset(IDirect3DDevice9* dev, D3DPRESENT_PARAMETERS* pp)
{
	D3D9DeviceHook* h = self_;
	if (h && h->onLost_)
	{
		h->onLost_(dev);
	}
	HRESULT hr = oReset(dev, pp);
	if (SUCCEEDED(hr) && h && h->onReset_)
	{
		h->onReset_(dev);
	}
	return hr;
}
