/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#include "dialog.hpp"

Dialog::Dialog() = default;

void Dialog::show(DialogShow d)
{
	current_ = std::move(d);
	visible_ = true;
}

void Dialog::hide()
{
	visible_ = false;
}

void Dialog::draw()
{
	// TODO: ImGui::OpenPopup + style-specific widget tree.
}
