/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#include "chat_window.hpp"

#include <imgui.h>

ChatWindow::ChatWindow() = default;

void ChatWindow::addLine(std::uint32_t argb, std::string text)
{
	buffer_.push_back({ argb, std::move(text), std::chrono::steady_clock::now() });
	if (buffer_.size() > maxBufferLines_)
	{
		buffer_.erase(buffer_.begin(), buffer_.begin() + (buffer_.size() - maxBufferLines_));
	}
}

void ChatWindow::clear()
{
	buffer_.clear();
}

void ChatWindow::setInputActive(bool active)
{
	inputActive_ = active;
}

void ChatWindow::draw()
{
	// TODO: ImGui::Begin with NoMove/NoTitleBar/NoBackground; render buffer_
	// as colour-tagged text and conditionally draw an input field.
}
