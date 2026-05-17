/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#include "loading_screen.hpp"

LoadingScreen::LoadingScreen() = default;

void LoadingScreen::setImage(std::string path)
{
	imagePath_ = std::move(path);
}

void LoadingScreen::draw()
{
	// TODO: fullscreen ImGui window; texture-load imagePath_ via stb_image and
	// upload once into an IDirect3DTexture9 owned by Overlay.
}
