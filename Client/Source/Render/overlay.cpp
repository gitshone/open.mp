/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#include "overlay.hpp"

#include "../UI/chat_window.hpp"
#include "../UI/dialog.hpp"
#include "../UI/loading_screen.hpp"
#include "../UI/scoreboard.hpp"
#include "../UI/text_draw.hpp"

#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>

Overlay::Overlay()
	: chat_(std::make_unique<ChatWindow>())
	, scoreboard_(std::make_unique<Scoreboard>())
	, dialog_(std::make_unique<Dialog>())
	, loadingScreen_(std::make_unique<LoadingScreen>())
	, textDraw_(std::make_unique<TextDraw>())
{
}

Overlay::~Overlay()
{
	if (backendReady_)
	{
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
}

void Overlay::onDeviceAcquired(IDirect3DDevice9* device, HWND window)
{
	if (backendReady_)
	{
		return;
	}
	device_ = device;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
	backendReady_ = true;
}

void Overlay::onDeviceLost()
{
	if (backendReady_)
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
	}
}

void Overlay::onDeviceReset()
{
	if (backendReady_)
	{
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}

void Overlay::render()
{
	if (!backendReady_)
	{
		return;
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Back-to-front: textdraws and HUD first, dialogs on top, loading screen owns
	// the full surface when active so it draws last.
	textDraw_->draw();
	chat_->draw();
	scoreboard_->draw();
	dialog_->draw();
	loadingScreen_->draw();

	// M1 watermark — proves the hook chain is alive end to end. Removed when
	// the rest of the UI is wired up.
	{
		ImGui::SetNextWindowPos(ImVec2(8.0f, 8.0f));
		ImGui::SetNextWindowBgAlpha(0.5f);
		constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize;
		if (ImGui::Begin("openmp_watermark", nullptr, flags))
		{
			ImGui::Text("open.mp client %d.%d.%d", OMP_CLIENT_VERSION_MAJOR, OMP_CLIENT_VERSION_MINOR, OMP_CLIENT_VERSION_PATCH);
		}
		ImGui::End();
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}
