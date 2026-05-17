/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#include "scoreboard.hpp"

#include <algorithm>

Scoreboard::Scoreboard() = default;

void Scoreboard::setRow(ScoreboardRow row)
{
	const auto it = std::find_if(rows_.begin(), rows_.end(),
		[id = row.playerId](const ScoreboardRow& r) { return r.playerId == id; });
	if (it != rows_.end())
	{
		*it = std::move(row);
	}
	else
	{
		rows_.push_back(std::move(row));
	}
}

void Scoreboard::removeRow(std::uint16_t playerId)
{
	std::erase_if(rows_, [playerId](const ScoreboardRow& r) { return r.playerId == playerId; });
}

void Scoreboard::clear()
{
	rows_.clear();
}

void Scoreboard::draw()
{
	// TODO: ImGui table widget with sortable columns.
}
