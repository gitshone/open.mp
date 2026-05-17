/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#pragma once

#include "../pch.hpp"

/// One on-screen textdraw. Field set matches Shared/NetCode/textdraw.hpp.
struct TextDrawInstance
{
	std::uint16_t id = 0;
	float x = 0.0f;
	float y = 0.0f;
	float letterWidth = 0.5f;
	float letterHeight = 1.0f;
	std::uint32_t letterColour = 0xFFFFFFFFu;
	std::uint32_t boxColour = 0;
	std::uint32_t backgroundColour = 0;
	std::uint8_t alignment = 1;
	std::uint8_t style = 0;
	bool proportional = false;
	bool selectable = false;
	bool useBox = false;
	float boxWidth = 0.0f;
	float boxHeight = 0.0f;
	std::string text;
};

/// Renderer for the textdraw RPC family (PlayerShowTextDraw / PlayerHideTextDraw,
/// IDs 134/135). Backed by Dear ImGui's draw list.
class TextDraw
{
public:
	TextDraw();

	void show(TextDrawInstance td);
	void hide(std::uint16_t id);
	void update(std::uint16_t id, std::string text);
	void clear();

	void draw();

private:
	std::vector<TextDrawInstance> active_;
};
