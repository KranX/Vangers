#ifndef NETWORK_CANCEL_H
#define NETWORK_CANCEL_H

#include "network.h"

// Parses a TOTAL_LIST_OF_PLAYERS_DATA response only when the blocking wait
// actually returned that event. Extracted so the cancellation guard and the
// real player-list parser can be exercised by tests.
bool process_total_players_response(
	int received_event,
	InputEventBuffer &events,
	PlayersList &players,
	int global_station_id,
	PlayerData *&my_player
);

// Resets the parts of the network session that must not survive a cancelled
// connect/configuration stage. Shared by all cancellation paths.
void reset_network_session_state(
	int &network_on,
	XSocket &socket,
	OutputEventBuffer &events_out,
	InputEventBuffer &events_in,
	PlayersList &players,
	PlayerData *&my_player
);

#endif
