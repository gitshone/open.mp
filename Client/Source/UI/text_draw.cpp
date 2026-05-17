/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#include "text_draw.hpp"

#include <algorithm>

TextDraw::TextDraw() = default;

void TextDraw::show(TextDrawInstance td)
{
	const auto it = std::find_if(active_.begin(), active_.end(),
		[id = td.id](const TextDrawInstance& a) { return a.id == id; });
	if (it != active_.end())
	{
		*it = std::move(td);
	}
	else
	{
		active_.push_back(std::move(td));
	}
}

void TextDraw::hide(std::uint16_t id)
{
	std::erase_if(active_, [id](const TextDrawInstance& a) { return a.id == id; });
}

void TextDraw::update(std::uint16_t id, std::string text)
{
	const auto it = std::find_if(active_.begin(), active_.end(),
		[id](const TextDrawInstance& a) { return a.id == id; });
	if (it != active_.end())
	{
		it->text = std::move(text);
	}
}

void TextDraw::clear()
{
	active_.clear();
}

void TextDraw::draw()
{
	// TODO: emit one ImGui draw-list entry per active_ row, respecting alignment
	// and box styling.
}
