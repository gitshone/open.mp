/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#pragma once

#include "../pch.hpp"

/// open.mp branded loading screen. Replaces GTA SA's default which is
/// suppressed via GameHooks.
class LoadingScreen
{
public:
	enum class Stage
	{
		BeforeGameLoaded,
		ConnectingToServer,
		JoiningWorld,
		Done,
	};

	LoadingScreen();

	void setImage(std::string path);
	void setTitle(std::string title) { title_ = std::move(title); }
	void setSubtitle(std::string sub) { subtitle_ = std::move(sub); }
	void setStage(Stage s) { stage_ = s; }
	void setProgress(float v) { progress_ = v; }

	void show() { visible_ = true; }
	void hide() { visible_ = false; }
	bool visible() const { return visible_; }

	void draw();

private:
	std::string imagePath_;
	std::string title_;
	std::string subtitle_;
	Stage stage_ = Stage::BeforeGameLoaded;
	float progress_ = 0.0f;
	bool visible_ = true;
};
