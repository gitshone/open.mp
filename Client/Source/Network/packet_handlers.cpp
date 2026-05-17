/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#include "packet_handlers.hpp"

#include "rpc_dispatcher.hpp"

void registerAllHandlers(RpcDispatcher& /*dispatcher*/)
{
	// Target shape once handlers exist (IDs match Shared/NetCode/*.hpp):
	//   dispatcher.registerRpc(61,  handleShowDialog);
	//   dispatcher.registerRpc(134, handlePlayerShowTextDraw);
	//   dispatcher.registerRpc(135, handlePlayerHideTextDraw);
	//   dispatcher.registerRpc(137, handlePlayerJoin);
}
