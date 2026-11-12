#include <iostream>

#include "multiplayer.h"
#include "network_query_gate.h"

namespace {

bool check(bool condition, const char *message) {
	if (!condition)
		std::cerr << message << '\n';
	return condition;
}

} // namespace

int main() {
	int parses = 0;
	auto parse = [&parses] { ++parses; };

	// Cancelled wait with an empty input buffer.
	if (!check(
			!run_network_query_parser(0, TOTAL_LIST_OF_PLAYERS_DATA, parse),
			"empty buffer cancellation was treated as a complete response"
		))
		return 1;

	// Cancelled wait while only a partial response was buffered.
	if (!check(
			!run_network_query_parser(0, TOTAL_LIST_OF_PLAYERS_DATA, parse),
			"partial buffer cancellation was treated as a complete response"
		))
		return 1;

	// A different event must not be parsed as the player list either.
	if (!check(
			!run_network_query_parser(GAME_DATA_RESPONSE, TOTAL_LIST_OF_PLAYERS_DATA, parse),
			"unrelated response was parsed as the player list"
		))
		return 1;

	if (!check(parses == 0, "parser ran without a complete player-list response"))
		return 1;

	// Cancelled configuration response waits must not be parsed as success.
	if (!check(
			!run_network_query_parser(0, GAME_DATA_RESPONSE, parse) &&
				!run_network_query_parser(0, zGAME_DATA_RESPONSE, parse),
			"cancelled configuration response was parsed"
		))
		return 1;

	if (!check(parses == 0, "configuration parser ran after cancellation"))
		return 1;

	if (!check(
			run_network_query_parser(TOTAL_LIST_OF_PLAYERS_DATA, TOTAL_LIST_OF_PLAYERS_DATA, parse),
			"complete player-list response was not parsed"
		))
		return 1;

	return check(parses == 1, "complete response was not parsed exactly once") ? 0 : 1;
}
