/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#pragma once

#include "../pch.hpp"

/// One line of chat history. ARGB encoding mirrors the SA-MP wire format.
struct ChatEntry
{
	std::uint32_t argbColour = 0xFFFFFFFFu;
	std::string text;
	std::chrono::steady_clock::time_point at = std::chrono::steady_clock::now();
};

/// In-game chat. Buffers a sliding window of recent messages and exposes an
/// input field gated by setInputActive(). Rendered through Dear ImGui.
class ChatWindow
{
public:
	ChatWindow();

	/// Appends one line and trims the buffer to maxBufferLines_.
	void addLine(std::uint32_t argb, std::string text);

	/// Drops the entire history.
	void clear();

	/// Toggles the input field. When active, all character/key input is consumed.
	void setInputActive(bool active);
	bool inputActive() const { return inputActive_; }

	/// Draws the current frame's chat surface.
	void draw();

private:
	std::vector<ChatEntry> buffer_;
	std::string inputBuffer_;
	bool inputActive_ = false;
	std::size_t maxBufferLines_ = 100;
};
