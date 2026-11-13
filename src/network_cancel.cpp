#include "network_cancel.h"

#include <cstring>

PlayerData::PlayerData(int client_id) {
	client_ID = client_id;
	status = 0;
	name = 0;
	x = y = 0;
	world = 0;
	GamingFlag = 0;
	next = prev = 0;
	list = 0;
	CreatePlayerFlag = 0;
	LastDestroyedNetID = 0;
	uvsPoint = NULL;
	for (int i = 0; i < MAX_ACTIVE_SLOT; i++) {
		SlotNetID[i] = 0;
		SlotStuffNetID[i] = 0;
		SlotFireCount[i] = 0;
	}
}

InputEventBuffer::InputEventBuffer(unsigned int size): XBuffer(size) {
	reset();
}

void InputEventBuffer::reset() {
	next_event_pointer = 0;
	filled_size = 0;
	offset = 0;
	event_ID = 0;
	client_ID = 0;
	item_state = 0;
	item_previous_ID = 0;
	item_removed_paired_ID = 0;
	item_removed_reason = 0;
	object_ID = 0;
}

OutputEventBuffer::OutputEventBuffer(unsigned int size): XBuffer(size) {
	clear();
}

void OutputEventBuffer::clear() {
	n_event = 0;
	offset = 0;
	pointer_to_the_first_event = 0;
	pointer_to_the_last_event = 0;
	pointer_to_size_of_event = -1;
}

void PlayersList::parsing_total_body_query(
	InputEventBuffer &events,
	int global_station_id,
	PlayerData *&my_player
) {
	unsigned char client_ID, status, world;
	short x, y;
	char name[257];
	PlayerData *p;
	delete_all();
	my_player = 0;
	int num = events.get_byte();
	for (int i = 0; i < num; i++) {
		events > client_ID > status > world > x > y > name;
		p = new PlayerData(client_ID);
		append(p);
		p->status = status;
		p->world = world;
		p->x = x;
		p->y = y;
		events.read(&p->body, sizeof(PlayerBody));
		if (!p->name) {
			p->name = new char[strlen(name) + 1];
			strcpy(p->name, name);
		} else {
			if (strcmp(p->name, name)) {
				delete p->name;
				p->name = new char[strlen(name) + 1];
				strcpy(p->name, name);
			}
		}
		if (p->client_ID == global_station_id)
			my_player = p;
	}
}

bool process_total_players_response(
	int received_event,
	InputEventBuffer &events,
	PlayersList &players,
	int global_station_id,
	PlayerData *&my_player
) {
	if (received_event != TOTAL_LIST_OF_PLAYERS_DATA)
		return false;
	players.parsing_total_body_query(events, global_station_id, my_player);
	return true;
}

void reset_network_session_state(
	int &network_on,
	XSocket &socket,
	OutputEventBuffer &events_out,
	InputEventBuffer &events_in,
	PlayersList &players,
	PlayerData *&my_player
) {
	network_on = 0;
	socket.close();
	events_out.clear();
	events_in.reset();
	players.delete_all();
	my_player = 0;
}
