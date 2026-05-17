/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2026, open.mp team and contributors.
 */

#include "net_client.hpp"

#include "rpc_dispatcher.hpp"

#include <spdlog/spdlog.h>

NetClient::NetClient()
	: dispatcher_(std::make_unique<RpcDispatcher>())
{
}

NetClient::~NetClient()
{
	disconnect();
}

bool NetClient::connect(ConnectArgs args)
{
	if (state_.load() != State::Disconnected)
	{
		return false;
	}
	args_ = std::move(args);
	state_.store(State::Connecting);
	worker_ = std::jthread([this](std::stop_token st) { run(st); });
	return true;
}

void NetClient::disconnect()
{
	if (state_.load() == State::Disconnected)
	{
		return;
	}
	state_.store(State::Disconnecting);
	worker_.request_stop();
	if (worker_.joinable())
	{
		worker_.join();
	}
	state_.store(State::Disconnected);
}

void NetClient::run(std::stop_token stop)
{
	spdlog::info("NetClient worker started for {}:{}", args_.host, args_.port);
	while (!stop.stop_requested())
	{
		// TODO: peer->Receive(); on success, dispatcher_->dispatchPacket / dispatchRpc.
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	spdlog::info("NetClient worker stopping");
}
