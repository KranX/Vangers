#include <cstring>
#include <iostream>
#include <memory>

#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>

#include "network.h"
#include "network_cancel.h"

namespace {

bool check(bool condition, const char *message) {
	if (!condition)
		std::cerr << "FAIL: " << message << '\n';
	return condition;
}

struct ServerDeleter {
	void operator()(NET_Server *server) const {
		NET_DestroyServer(server);
	}
};

struct PeerDeleter {
	void operator()(NET_StreamSocket *peer) const {
		NET_DestroyStreamSocket(peer);
	}
};

using TestServer = std::unique_ptr<NET_Server, ServerDeleter>;
using TestPeer = std::unique_ptr<NET_StreamSocket, PeerDeleter>;

struct PlayerListFixture {
	PlayersList players;

	~PlayerListFixture() {
		players.delete_all();
	}
};

int listen_on_available_port(TestServer &server) {
	for (int port = 32397; port < 32497; ++port) {
		server.reset(NET_CreateServer(nullptr, static_cast<Uint16>(port), 0));
		if (server)
			return port;
	}
	return 0;
}

TestPeer wait_for_client(const TestServer &server) {
	const Uint64 deadline = SDL_GetTicks() + 2000;
	do {
		NET_StreamSocket *client = nullptr;
		if (!NET_AcceptClient(server.get(), &client))
			return {};
		if (client)
			return TestPeer(client);
		SDL_Delay(1);
	} while (SDL_GetTicks() < deadline);
	return {};
}

bool run_player_list_cancellation_test() {
	InputEventBuffer events(1024);
	PlayerListFixture fixture;
	PlayersList &players = fixture.players;
	PlayerData *existing = new PlayerData(3);
	players.append(existing);
	PlayerData *my_player = existing;

	// Cancellation with an empty input buffer: the parser must not run.
	if (!check(
			!process_total_players_response(0, events, players, 1, my_player),
			"empty cancelled response was treated as complete"
		))
		return false;
	if (!check(
			players.size() == 1 && players.first() == existing && my_player == existing &&
				events.tell() == 0,
			"empty cancelled response modified the player list"
		))
		return false;

	// Cancellation with a partial response buffered: still no parsing.
	events < char(2) < char(1) < char(0) < char(0);
	events.set(0);
	if (!check(
			!process_total_players_response(0, events, players, 1, my_player),
			"partial cancelled response was treated as complete"
		))
		return false;
	if (!check(
			players.size() == 1 && players.first() == existing && my_player == existing &&
				events.tell() == 0,
			"partial cancelled response modified the player list"
		))
		return false;

	// A complete response must still parse normally.
	events.reset();
	PlayerBody body;
	std::memset(&body, 0, sizeof(body));
	body.kills = 4;
	body.color = 3;
	body.world = 2;
	body.beebos = 100;
	body.rating = 1.25f;
	body.NetID = 12345;
	events < char(1) < char(7) < char(1) < char(2) < short(10) < short(20) < "tester" < char(0);
	events.write(&body, sizeof(body));
	const auto response_size = events.tell();
	events.set(0);
	if (!check(
			process_total_players_response(
				TOTAL_LIST_OF_PLAYERS_DATA, events, players, 7, my_player
			),
			"complete player-list response was not parsed"
		))
		return false;

	PlayerData *parsed = players.first();
	if (!check(
			players.size() == 1 && parsed != nullptr && parsed->client_ID == 7 &&
				my_player == parsed && parsed->status == 1 && parsed->world == 2 &&
				parsed->x == 10 && parsed->y == 20 && parsed->name != nullptr &&
				std::strcmp(parsed->name, "tester") == 0 &&
				std::memcmp(&parsed->body, &body, sizeof(body)) == 0,
			"parsed player list does not match the response"
		))
		return false;
	if (!check(events.tell() == response_size, "parser did not consume exactly the response"))
		return false;

	return true;
}

// Exercises the shared cleanup operation, not configuration-response waits.
bool run_session_reset_test() {
	TestServer server;
	const int port = listen_on_available_port(server);
	if (!check(port != 0, "could not allocate a session reset test port"))
		return false;

	XSocket client;
	if (!check(client.open("127.0.0.1", port), "session reset client connection failed"))
		return false;
	TestPeer accepted = wait_for_client(server);
	if (!check(static_cast<bool>(accepted), "session reset peer was not accepted"))
		return false;

	int network_on = 1;
	OutputEventBuffer events_out(1024);
	events_out < short(0) < char(GAMES_LIST_QUERY);
	InputEventBuffer events_in(1024);
	events_in < char(0xAA) < char(0xBB);

	PlayerListFixture fixture;
	PlayersList &players = fixture.players;
	PlayerData *player = new PlayerData(1);
	players.append(player);
	PlayerData *my_player = player;

	if (!check(events_out.tell() > 0, "output buffer was not populated"))
		return false;
	if (!check(events_in.tell() > 0, "input buffer was not populated"))
		return false;

	reset_network_session_state(network_on, client, events_out, events_in, players, my_player);

	if (!check(network_on == 0, "cancelled session kept NetworkON"))
		return false;
	if (!check(!client.is_open(), "cancelled session kept the socket open"))
		return false;
	if (!check(events_out.tell() == 0, "cancelled session kept the output buffer"))
		return false;
	if (!check(events_in.tell() == 0, "cancelled session kept the input buffer"))
		return false;
	if (!check(players.first() == nullptr, "cancelled session kept player data"))
		return false;
	if (!check(my_player == nullptr, "cancelled session kept my_player_data"))
		return false;

	return true;
}

} // namespace

int main() {
	if (!check(SDL_Init(0), "SDL initialization failed"))
		return 1;
	if (!check(XSocketInit(0), "SDL3_net initialization failed"))
		return 1;

	const bool success = run_player_list_cancellation_test() && run_session_reset_test();

	XSocketFinit();
	SDL_Quit();
	return success ? 0 : 1;
}
