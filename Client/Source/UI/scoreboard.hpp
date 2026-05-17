/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#pragma once

#include "../pch.hpp"

/// Single scoreboard entry. Mirrors what the server sends through the player
/// pool RPCs.
struct ScoreboardRow
{
	std::uint16_t playerId = 0;
	std::string name;
	int score = 0;
	int ping = 0;
	std::uint32_t colour = 0xFFFFFFFFu;
};

/// TAB-toggled player list. Renders through Dear ImGui.
class Scoreboard
{
public:
	Scoreboard();

	/// Inserts or replaces the row with the matching playerId.
	void setRow(ScoreboardRow row);

	/// Drops the row with the matching playerId, if present.
	void removeRow(std::uint16_t playerId);

	void clear();

	void show() { visible_ = true; }
	void hide() { visible_ = false; }
	bool visible() const { return visible_; }

	void draw();

private:
	std::vector<ScoreboardRow> rows_;
	bool visible_ = false;
};
