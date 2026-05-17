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

/// Outbound connection parameters.
struct ConnectArgs
{
	std::string host;
	std::uint16_t port = 7777;
	std::string nickname;
	std::string password;
	std::uint32_t versionNumber = 0;
	std::uint32_t ompClientVersion = 0;
};

/// Owns the RakNet client peer, drives its poll loop on a dedicated jthread,
/// and routes received traffic through RpcDispatcher. Speaks the wire format
/// defined by open.mp/Shared/NetCode/ — identical to SA-MP.
class NetClient
{
public:
	enum class State
	{
		Disconnected,
		Connecting,
		Authenticating,
		Connected,
		Disconnecting,
	};

	NetClient();
	~NetClient();

	NetClient(const NetClient&) = delete;
	NetClient& operator=(const NetClient&) = delete;

	/// Begins the connection sequence. Non-blocking.
	bool connect(ConnectArgs args);

	/// Closes the peer and joins the worker.
	void disconnect();

	State state() const { return state_.load(); }
	RpcDispatcher& dispatcher() { return *dispatcher_; }

private:
	/// Drives RakNet's Receive() loop and hands packets to the dispatcher.
	void run(std::stop_token stop);

	std::atomic<State> state_ { State::Disconnected };
	std::jthread worker_;
	std::unique_ptr<RpcDispatcher> dispatcher_;
	ConnectArgs args_;
};
