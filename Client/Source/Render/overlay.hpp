/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#pragma once

#include "../pch.hpp"

class ChatWindow;
class Scoreboard;
class Dialog;
class LoadingScreen;
class TextDraw;

/// Owns the Dear ImGui context and its DX9/Win32 backends. UI subsystems
/// register themselves here and contribute one ImGui block per frame; the
/// overlay flushes a single draw list through the device.
class Overlay
{
public:
	Overlay();
	~Overlay();

	Overlay(const Overlay&) = delete;
	Overlay& operator=(const Overlay&) = delete;

	/// Hands the captured device to ImGui's D3D9 backend. Idempotent.
	/// @param device Live IDirect3DDevice9 owned by the game.
	/// @param window HWND for the Win32 backend.
	void onDeviceAcquired(IDirect3DDevice9* device, HWND window);

	/// Releases ImGui's device objects (called from D3D9::Reset's lost path).
	void onDeviceLost();

	/// Recreates ImGui's device objects after the device has been reset.
	void onDeviceReset();

	/// Drives one frame. Call from inside the EndScene trampoline.
	void render();

	ChatWindow& chat() { return *chat_; }
	Scoreboard& scoreboard() { return *scoreboard_; }
	Dialog& dialog() { return *dialog_; }
	LoadingScreen& loadingScreen() { return *loadingScreen_; }
	TextDraw& textDraw() { return *textDraw_; }

private:
	IDirect3DDevice9* device_ = nullptr;
	bool backendReady_ = false;

	std::unique_ptr<ChatWindow> chat_;
	std::unique_ptr<Scoreboard> scoreboard_;
	std::unique_ptr<Dialog> dialog_;
	std::unique_ptr<LoadingScreen> loadingScreen_;
	std::unique_ptr<TextDraw> textDraw_;
};
