/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#pragma once

#include "../pch.hpp"

class RpcDispatcher;

/// Wires every concrete RPC/packet handler against the dispatcher. Called once
/// during NetClient initialisation; nothing here owns state beyond the
/// references it captures.
void registerAllHandlers(RpcDispatcher& dispatcher);
