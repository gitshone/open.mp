/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#include "rpc_dispatcher.hpp"

void RpcDispatcher::registerRpc(std::uint8_t id, RawHandler handler)
{
	rpcHandlers_[id] = std::move(handler);
}

void RpcDispatcher::registerPacket(std::uint8_t id, RawHandler handler)
{
	packetHandlers_[id] = std::move(handler);
}

void RpcDispatcher::dispatchRpc(std::uint8_t id, std::span<const std::uint8_t> payload)
{
	if (auto& h = rpcHandlers_[id])
	{
		h(payload);
	}
}

void RpcDispatcher::dispatchPacket(std::uint8_t id, std::span<const std::uint8_t> payload)
{
	if (auto& h = packetHandlers_[id])
	{
		h(payload);
	}
}
