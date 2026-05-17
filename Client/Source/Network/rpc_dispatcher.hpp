/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#pragma once

#include "../pch.hpp"

/// Routes inbound RakNet packets and RPCs to handlers registered by client
/// subsystems. RPC IDs match Shared/NetCode/*.hpp so a stock SA-MP server is
/// fully understood without translation. open.mp-only RPCs occupy the
/// reserved range [200, 255] and are emitted only when the server saw a
/// non-zero OmpVersion on PlayerConnect.
class RpcDispatcher
{
public:
	using RawHandler = std::function<void(std::span<const std::uint8_t>)>;

	void registerRpc(std::uint8_t id, RawHandler handler);
	void registerPacket(std::uint8_t id, RawHandler handler);

	void dispatchRpc(std::uint8_t id, std::span<const std::uint8_t> payload);
	void dispatchPacket(std::uint8_t id, std::span<const std::uint8_t> payload);

private:
	std::array<RawHandler, 256> rpcHandlers_ {};
	std::array<RawHandler, 256> packetHandlers_ {};
};
