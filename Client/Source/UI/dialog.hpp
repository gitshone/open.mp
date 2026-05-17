/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#pragma once

#include "../pch.hpp"

/// SA-MP dialog styles. Wire-compatible with Shared/NetCode/dialog.hpp.
enum class DialogStyle : std::uint8_t
{
	MsgBox = 0,
	Input = 1,
	List = 2,
	Password = 3,
	TabList = 4,
	TabListHeaders = 5,
};

/// Payload of an inbound ShowDialog RPC (ID 61).
struct DialogShow
{
	int id = -1;
	DialogStyle style = DialogStyle::MsgBox;
	std::string title;
	std::string firstButton;
	std::string secondButton;
	std::string body;
};

/// Payload sent back to the server as OnPlayerDialogResponse (RPC ID 62).
struct DialogResponse
{
	int id = -1;
	std::uint8_t response = 0;
	int listItem = -1;
	std::string text;
};

/// Modal dialog renderer.
class Dialog
{
public:
	using Responder = std::function<void(const DialogResponse&)>;

	Dialog();

	void setResponder(Responder r) { responder_ = std::move(r); }

	/// Displays a dialog from the server. Replaces any currently-shown dialog.
	void show(DialogShow d);

	void hide();
	bool visible() const { return visible_; }

	void draw();

private:
	DialogShow current_;
	bool visible_ = false;
	Responder responder_;
};
