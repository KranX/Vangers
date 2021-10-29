#include "kdsplus.h"
#include "xerrhand.h"

#define LAG -3000
//#define EVENTS_LOG
#define SERVER_VERSION 1

#ifdef EVENTS_LOG
XStream fout("lst", XS_OUT);
#	define IN_EVENTS_LOG(id)                                                                    \
		{                                                                                        \
			fout < "\t\t\t> [" <= ID < ":" <= (GLOBAL_CLOCK() >> 8) < ":" <= frame < "]" < #id < \
				"\n";                                                                            \
		}
#	define IN_EVENTS_LOG1(id, code)                                                             \
		{                                                                                        \
			fout < "\t\t\t> [" <= ID < ":" <= (GLOBAL_CLOCK() >> 8) < ":" <= frame < "]" < #id < \
				": " <= code < "\n";                                                             \
		}
#	define OUT_EVENTS_LOG(id) \
		{ fout < "< [" <= ID < ":" <= (GLOBAL_CLOCK() >> 8) < ":" <= frame < "]" < #id < "\n"; }
#	define OUT_EVENTS_LOG1(id, code)                                                              \
		{                                                                                          \
			fout < "< [" <= ID < ":" <= (GLOBAL_CLOCK() >> 8) < ":" <= frame < "]" < #id < ": " <= \
				code < "\n";                                                                       \
		}
#else
#	define IN_EVENTS_LOG(id)
#	define IN_EVENTS_LOG1(id, code)
#	define OUT_EVENTS_LOG(id)
#	define OUT_EVENTS_LOG1(id, code)
#endif

#ifdef _DEBUG
#	define DOUT(str) \
		{ std::cout << str << "                                       \n"; }
#	define DOUT1(str, code) \
		{ std::cout << str << ", code: " << code << "                                       \n"; }
#else
#	define DOUT(str)
#	define DOUT1(str, code)
#endif
#define MOUT(str) \
	{ std::cout << str << "                                       \n"; }
#define MOUT1(str, code) \
	{ std::cout << str << ", code: " << code << "                                       \n"; }

const char *MP_GAMES_NAMES[NUMBER_MP_GAMES] = {"VAN_WAR", "MECHOSOMA", "PASSEMBLOSS"};

XStream stat_log;

/******************************************************************
				Game
******************************************************************/
ServerData::ServerData() {
	memset(this, 0, sizeof(ServerData));
	GameType = UNCONFIGURED;
}

Game::Game(int ID) {
	Game::ID = ID;
	name[0] = 0;
	client_version = 0;
	birth_time = SDL_GetTicks();
	next = prev = 0;
	list = 0;
	used_players_IDs = 0;
}

Game::~Game() {
#ifdef _DEBUG
	save_result();
#endif

	if (players.size() || removed_players.size()) {
		switch (data.GameType) {
		case VAN_WAR:
			process_VAN_WAR_ratings();
			break;
		case MECHOSOMA:
			process_MECHOSOMA_ratings();
			break;
		case PASSEMBLOSS:
			process_PASSEMBLOSS_ratings();
			break;
		}
	}

	Player *p;
	while ((p = players.first()) != 0) {
		detach_player(p);
		delete p;
	}
	removed_players.delete_all();

	World *w;
	while ((w = worlds.first()) != 0) {
		worlds.remove(w);
		delete w;
	}

	DOUT1("Clear game", ID);
}

int Game::attach_player(Player *player) {
	for (int i = 0; i < 31; i++)
		if (!(used_players_IDs & (1 << i))) {
			used_players_IDs |= 1 << i;
			player->server->clients.remove(player);
			players.append(player);
			player->client_mask = 1 << i;
			player->game = this;
			DOUT1("Player attached", i + 1);
			return player->ID = i + 1;
		}
	return 0;
}

void Game::detach_player(Player *player) {
	//	if(player -> name && player -> password && player -> status == GAMING_STATUS)
	//		player -> server -> add_rating_data(player -> name,player ->
	//password,data.GameType,player
	//-> rating);

	player->clear_object_queue(0);

	if (player->world)
		player->world->detach_player(player);

	players.remove(player);
	player->client_mask = 0;
	player->game = 0;
	DOUT1("Player detached", player->ID);
	//	player -> ID = 0;
}

void Game::get_object_ID_offsets(OutputEventBuffer &out_buffer, int client_ID) {
	int offsets[16];
	memset(offsets, 0, 16 * sizeof(int));
	unsigned int inv_mask = ~client_ID;
	Object *obj;
	World *w = worlds.first();
	while (w) {
		obj = w->objects.first();
		while (obj) {
			if (CLIENT_ID(obj->ID) == client_ID &&
				offsets[(obj->ID >> 16) & 63] < (obj->ID & 0xffff))
				offsets[(obj->ID >> 16) & 63] = obj->ID & 0xffff;
			obj = obj->next_alt;
		}
		w = w->next;
	}
	Player *p = players.first();
	while (p) {
		obj = p->inventory.first();
		while (obj) {
			if (CLIENT_ID(obj->ID) == client_ID &&
				offsets[(obj->ID >> 16) & 63] < (obj->ID & 0xffff))
				offsets[(obj->ID >> 16) & 63] = obj->ID & 0xffff;
			obj = obj->next;
		}
		p = p->next;
	}
	obj = global_objects.first();
	while (obj) {
		if (CLIENT_ID(obj->ID) == client_ID && offsets[(obj->ID >> 16) & 63] < (obj->ID & 0xffff))
			offsets[(obj->ID >> 16) & 63] = obj->ID & 0xffff;

		obj->visibility &= inv_mask;
		obj->last_update &= inv_mask;
		obj->in_queue &= inv_mask;
		obj->send_hide &= inv_mask;
		obj->send_delete &= inv_mask;

		obj = obj->next;
	}

	for (int i = 0; i < 16; i++)
		out_buffer < (unsigned short)(offsets[i] ? offsets[i] + 1 : 0);
}

int Game::quant() {
	int work_log = 0;
	Player *p = players.first();
	while (p) {
		work_log += p->receive();
		if (!p->is_alive()) {
			Player *p_next = p->next;
			used_players_IDs &= ~(1 << (p->ID - 1));
			detach_player(p);
			if (p->status == GAMING_STATUS) {
				p->status = FINISHED_STATUS;
				put_event_for_all(PLAYERS_STATUS, p);
				removed_players.append(p);
				switch (data.GameType) {
				case VAN_WAR:
					process_VAN_WAR_ratings();
					break;
				case MECHOSOMA:
					process_MECHOSOMA_ratings();
					break;
				case PASSEMBLOSS:
					process_PASSEMBLOSS_ratings();
					break;
				}
			} else
				delete p;
			p = p_next;
			continue;
		}
		p = p->next;
	}

	p = players.first();
	while (p) {
		work_log += p->send();
		p = p->next;
	}

	return work_log;
}

void Game::put_event_for_all(int code, Player *player) {
	Player *p = players.first();
	while (p) {
		if (p != player)
			p->code_queue.put(Event(code, player));
		p = p->next;
	}
}

void Game::process_direct_sending(Object *obj, unsigned int mask) {
	Player *p = players.first();
	while (p) {
		if (mask & p->client_mask) {
			obj->send_delete |= p->client_mask;
			p->put_object(obj);
		}
		p = p->next;
	}
}

void Game::get_total_list_of_players_data(OutputEventBuffer &out_buffer) {
	out_buffer.begin_event(TOTAL_LIST_OF_PLAYERS_DATA);
	out_buffer < (unsigned char)(players.size() + removed_players.size());
	Player *p = players.first();
	while (p) {
		out_buffer < (unsigned char)(p->ID) < (unsigned char)p->status <
			(unsigned char)(p->world ? p->world->ID : 0) < short(p->x) < short(p->y) < p->name <
			char(0);
		out_buffer.write((unsigned char *)&p->body, sizeof(PlayerBody));
		p = p->next;
	}
	p = removed_players.first();
	while (p) {
		out_buffer < (unsigned char)(p->ID) < (unsigned char)p->status <
			(unsigned char)(p->world ? p->world->ID : 0) < short(p->x) < short(p->y) < p->name <
			char(0);
		out_buffer.write((unsigned char *)&p->body, sizeof(PlayerBody));
		p = p->next;
	}
	out_buffer.end_event();
}

void Game::check_global_objects(Player *player) {
	Object *obj = global_objects.first();
	unsigned int mask = player->client_mask;
	while (obj) {
		if (!(obj->last_update & mask)) {
			player->put_object(obj);
			obj->last_update |= mask;
		}
		obj = obj->next;
	}
}

void Game::process_create_globals(Player *player, Object *obj) {
	global_objects.append(obj);
	obj->list = &global_objects;
	Player *p = players.first();
	while (p) {
		if (p != player) {
			p->put_object(obj);
			obj->last_update |= p->client_mask;
		} else
			obj->last_update |= p->client_mask;
		p = p->next;
	}
}

void Game::process_update_globals(Player *player, Object *obj) {
	obj->last_update = 0;
	Player *p = players.first();
	while (p) {
		if (p != player) {
			p->put_object(obj);
			obj->last_update |= p->client_mask;
		} else
			obj->last_update |= p->client_mask;
		p = p->next;
	}
}

void Game::process_delete_globals(Object *obj) {
	Player *p = players.first();
	while (p) {
		obj->send_delete |= p->client_mask;
		p->put_object(obj);
		p = p->next;
	}
}

void Game::process_VAN_WAR_ratings() {
	int total_account = 0;
	int counter = 0;
	double avr_rating = 0;
	Player *p = players.first();
	while (p) {
		if (p->status != INITIAL_STATUS) {
			p->body.rating = (float)((p->body.kills - p->body.deaths));
			total_account += p->body.kills;
			avr_rating += p->body.rating;
			counter++;
		}
		p = p->next;
	}
	p = removed_players.first();
	while (p) {
		p->body.rating = (float)((p->body.kills - p->body.deaths));
		total_account += p->body.kills;
		avr_rating += p->body.rating;
		counter++;
		p = p->next;
	}
	if (!counter)
		return;
	avr_rating /= (double)counter;

	double total_weight = 0;
	p = players.first();
	while (p) {
		if (p->status != INITIAL_STATUS)
			total_weight += fabs(p->body.rating - avr_rating);
		p = p->next;
	}
	p = removed_players.first();
	while (p) {
		total_weight += fabs(p->body.rating - avr_rating);
		p = p->next;
	}
	if (total_weight < 0.01)
		return;
	total_weight /= 2.;

	double factor = (double)total_account / (total_weight);
	p = players.first();
	while (p) {
		if (p->status != INITIAL_STATUS) {
			p->body.rating = (float)((p->body.rating - avr_rating) * factor);
			p->server->add_rating_data(p, VAN_WAR);
		}
		p = p->next;
	}
	p = removed_players.first();
	while (p) {
		p->body.rating = (float)((p->body.rating - avr_rating) * factor);
		p->server->add_rating_data(p, VAN_WAR);
		p = p->next;
	}
}

void Game::process_MECHOSOMA_ratings() {
	int counter = 0;
	double avr_rating = 0;
	Player *p = players.first();
	while (p) {
		if (p->status != INITIAL_STATUS) {
			p -> body.rating = (float)((p -> body.MechosomaStat.MaxTransitTime)*pow(1.1,p -> body.kills)*pow(0.9,p -> body.deaths));
			avr_rating += p->body.rating;
			counter++;
		}
		p = p->next;
	}
	p = removed_players.first();
	while (p) {
		p -> body.rating = (float)((p -> body.MechosomaStat.MaxTransitTime)*pow(1.1,p -> body.kills)*pow(0.9,p -> body.deaths));
		avr_rating += p->body.rating;
		counter++;
		p = p->next;
	}
	if (!counter)
		return;
	avr_rating /= (double)counter;

	double total_weight = 0;
	p = players.first();
	while (p) {
		if (p->status != INITIAL_STATUS)
			total_weight += fabs(p->body.rating - avr_rating);
		p = p->next;
	}
	p = removed_players.first();
	while (p) {
		total_weight += fabs(p->body.rating - avr_rating);
		p = p->next;
	}
	if (total_weight < 0.01)
		return;
	total_weight /= 2.;

	double total_account =
		2. * (double)(data.Mechosoma.ProductQuantity1 + data.Mechosoma.ProductQuantity2) * counter;
	//	if(data.Mechosoma.One_at_a_time)
	//		rating *= 2.;
	double factor = total_account / total_weight;
	p = players.first();
	while (p) {
		if (p->status != INITIAL_STATUS) {
			p->body.rating = (float)((p->body.rating - avr_rating) * factor);
			p->server->add_rating_data(p, MECHOSOMA);
		}
		p = p->next;
	}
	p = removed_players.first();
	while (p) {
		p->body.rating = (float)((p->body.rating - avr_rating) * factor);
		p->server->add_rating_data(p, MECHOSOMA);
		p = p->next;
	}
}

void Game::process_PASSEMBLOSS_ratings() {
	int counter = 0;
	double avr_rating = 0;
	Player *p = players.first();
	while (p) {
		if (p->status != INITIAL_STATUS) {
			p -> body.rating = (float)((p -> body.PassemblossStat.TotalTime)*pow(1.1,p -> body.kills)*pow(0.9,p -> body.deaths));
			avr_rating += p->body.rating;
			counter++;
		}
		p = p->next;
	}
	p = removed_players.first();
	while (p) {
		p->body.rating =
			(float)((p->body.PassemblossStat.TotalTime) * pow(1.1, p->body.kills) * pow(0.9, p->body.deaths));
		avr_rating += p->body.rating;
		counter++;
		p = p->next;
	}
	if (!counter)
		return;
	avr_rating /= (double)counter;

	double total_weight = 0;
	p = players.first();
	while (p) {
		if (p->status != INITIAL_STATUS)
			total_weight += fabs(p->body.rating - avr_rating);
		p = p->next;
	}
	p = removed_players.first();
	while (p) {
		total_weight += fabs(p->body.rating - avr_rating);
		p = p->next;
	}
	if (total_weight < 0.01)
		return;
	total_weight /= 2.;

	double total_account = 5. * (double)(data.Passembloss.CheckpointsNumber * counter);
	double factor = total_account / total_weight;
	p = players.first();
	while (p) {
		if (p->status != INITIAL_STATUS) {
			p->body.rating = (float)((p->body.rating - avr_rating) * factor);
			p->server->add_rating_data(p, PASSEMBLOSS);
		}
		p = p->next;
	}
	p = removed_players.first();
	while (p) {
		p->body.rating = (float)((p->body.rating - avr_rating) * factor);
		p->server->add_rating_data(p, PASSEMBLOSS);
		p = p->next;
	}
}

void Game::save_result() {
	if (!players.size() && !removed_players.size())
		return;
	XBuffer name;
	name <= time(0) < ".rs" <= data.GameType;
	XStream out(name.GetBuf(), XS_OUT);

	out.write(&data, sizeof(ServerData));
	int counter = 0;
	int offset = out.tell();
	out < counter;
	Player *p = players.first();
	while (p) {
		if (p->status != INITIAL_STATUS) {
			out < p->name < char(0);
			out.write(&p->body, sizeof(PlayerBody));
			counter++;
		}
		p = p->next;
	}
	p = removed_players.first();
	while (p) {
		out < p->name < char(0);
		out.write(&p->body, sizeof(PlayerBody));
		counter++;
		p = p->next;
	}
	out.seek(offset, XS_BEG);
	out < counter;
}
void Game::load_result(Server *server, char *name) {
	players.delete_all();
	removed_players.delete_all();
	XStream in(name, XS_IN);
	in.read(&data, sizeof(ServerData));
	int counter;
	in > counter;
	XSocket sock;
	for (int i = 0; i < counter; i++) {
		Player *p = new Player(server, sock);
		removed_players.append(p);
		p->name = new char[256];
		in > p->name;
		in.read(&p->body, sizeof(PlayerBody));
	}
	switch (data.GameType) {
	case VAN_WAR:
		process_VAN_WAR_ratings();
		break;
	case MECHOSOMA:
		process_MECHOSOMA_ratings();
		break;
	case PASSEMBLOSS:
		process_PASSEMBLOSS_ratings();
		break;
	}
}

/******************************************************************
				World
******************************************************************/
World::World(int id, int Vsize) {
	ID = id;
	number_of_objects = 0;
	y_lists = new XTList<Object>[number_of_y_lists = (V_SIZE = Vsize) >> Y_SHIFT];
	next = prev = 0;
	list = 0;
}
World::~World() {
	current_players.clear();
	Object *obj;
	while ((obj = objects.first()) != 0) {
		delete_object(obj);
		delete obj;
	}
	delete[] y_lists;
}

void World::attach_player(Player *player) {
	player->world = this;
	current_players.append(player);
	player->x = player->y = player->y_half_size_of_screen = 0;
	player->x_prev = player->y_prev = player->y_half_size_of_screen_prev = 0;
	if (!player->status) {
		player->status = GAMING_STATUS;
		player->code_queue.put(Event(PLAYERS_STATUS, player));
		player->game->put_event_for_all(PLAYERS_STATUS, player);
	}
	player->game->put_event_for_all(PLAYERS_WORLD, player);
}

void World::detach_player(Player *player) {
	player->clear_object_queue(1);

	// process_delete below won't place any objects to this player
	current_players.remove(player);
	player->world = 0;

	// Clear Inventory
	Object *obj = player->inventory.first();
	while (obj) {
		process_delete(obj);
		OUT_EVENTS_LOG1(AUTO_DELETE_INVENTORY, obj->ID);
		obj = obj->next;
	}
	// Clear Update Info and Private World's Objects
	int client_ID = player->ID;
	int inv_mask = ~player->client_mask;
	obj = objects.first();
	while (obj) {
		if (CLIENT_ID(obj->ID) == client_ID && PRIVATE_OBJECT(obj->ID)) {
			process_delete(obj);
			OUT_EVENTS_LOG1(AUTO_DELETE, obj->ID);
		} else {
			obj->visibility &= inv_mask;
			obj->last_update &= inv_mask;
			obj->in_queue &= inv_mask;
			obj->send_hide &= inv_mask;
			obj->send_delete &= inv_mask;
		}
		obj = obj->next_alt;
	}

	// Clear Update Info of Inventories
	Player *p = current_players.first();
	while (p) {
		obj = p->inventory.first();
		while (obj) {
			obj->visibility &= inv_mask;
			obj->last_update &= inv_mask;
			obj->in_queue &= inv_mask;
			obj->send_hide &= inv_mask;
			obj->send_delete &= inv_mask;
			obj = obj->next;
		}
		p = p->next_alt;
	}

	player->game->put_event_for_all(PLAYERS_WORLD, player);
}

Object *World::search_object(int ID) {
	return objects.search(ID);
}

void World::add_object(Object *obj) {
	number_of_objects++;
	objects.append(obj);
	XTList<Object> *list = y_lists + ((obj->y >> Y_SHIFT) & (number_of_y_lists - 1));
	list->append(obj);
	obj->list = list;
}

void World::move_object(Object *obj) {
	XTList<Object> *new_list = y_lists + ((obj->y >> Y_SHIFT) & (number_of_y_lists - 1));
	if (new_list != obj->list) {
		obj->list->remove(obj);
		new_list->append(obj);
		obj->list = new_list;
	}
}

void World::delete_object(Object *obj) {
	number_of_objects--;
	objects.remove(obj);
	if (obj->list) {
		obj->list->remove(obj);
		obj->list = 0;
	}
}

int World::getDistY(int v0, int v1) {
	int d = v0 - v1;
	int ad = abs(d);
	int dd = V_SIZE - ad;
	if (ad <= dd)
		return d;
	return d < 0 ? d + V_SIZE : d - V_SIZE;
}

int World::check_visibility(Player *player, Object *object) {
	if (abs(getDistY(object->y, player->y)) < player->y_half_size_of_screen + object->radius)
		return 1;
	return 0;
}

void World::process_create(Player *player, Object *obj) {
	add_object(obj);
	Player *p = current_players.first();
	while (p) {
		if (p != player) {
			if (check_visibility(p, obj)) {
				p->put_object(obj);
				obj->visibility |= p->client_mask;
				obj->last_update |= p->client_mask;
			}
		} else {
			if (check_visibility(p, obj))
				obj->visibility |= p->client_mask;
			obj->last_update |= p->client_mask;
		}
		p = p->next_alt;
	}
}

void World::process_update(Player *player, Object *obj) {
	obj->last_update = 0;
	Player *p = current_players.first();
	if (NON_STATIC(obj->ID)) {
		move_object(obj);
		while (p) {
			if (check_visibility(p, obj)) {
				if (p != player)
					p->put_object(obj);
				obj->last_update |= p->client_mask;
				obj->visibility |= p->client_mask;
				obj->send_hide &= ~(p->client_mask);
			} else {
				if (obj->visibility & p->client_mask) {
					p->put_object(obj); // Hide
					obj->send_hide |= p->client_mask;
				}
				obj->visibility &= ~(p->client_mask);
				if (p == player)
					obj->last_update |= p->client_mask;
			}
			p = p->next_alt;
		}
	} else {
		while (p) {
			if (check_visibility(p, obj)) {
				if (p != player)
					p->put_object(obj);
				obj->last_update |= p->client_mask;
				obj->visibility |= p->client_mask;
			} else {
				obj->visibility &= ~(p->client_mask);
				if (p == player)
					obj->last_update |= p->client_mask;
			}
			p = p->next_alt;
		}
	}
}

void World::process_delete(Object *obj) {
	Player *p = current_players.first();
	while (p) {
		obj->send_delete |= p->client_mask;
		p->put_object(obj);
		p = p->next_alt;
	}
}

void World::process_set_position(Player *player) {
	Object *obj;
	unsigned int mask = player->client_mask;
	if (player->y_half_size_of_screen_prev) { // not a first set_position
		int update_y0, update_y1;
		int hide_y0, hide_y1;
		if (getDistY(player->y, player->y_prev) > 0) {
			update_y0 =
				((player->y_prev + player->y_half_size_of_screen_prev) & (V_SIZE - 1)) >> Y_SHIFT;
			update_y1 = ((player->y + player->y_half_size_of_screen) & (V_SIZE - 1)) >> Y_SHIFT;
			hide_y0 =
				((player->y_prev - player->y_half_size_of_screen_prev) & (V_SIZE - 1)) >> Y_SHIFT;
			hide_y1 = ((player->y - player->y_half_size_of_screen) & (V_SIZE - 1)) >> Y_SHIFT;
		} else {
			update_y1 =
				((player->y_prev - player->y_half_size_of_screen_prev) & (V_SIZE - 1)) >> Y_SHIFT;
			update_y0 = ((player->y - player->y_half_size_of_screen) & (V_SIZE - 1)) >> Y_SHIFT;
			hide_y1 =
				((player->y_prev + player->y_half_size_of_screen_prev) & (V_SIZE - 1)) >> Y_SHIFT;
			hide_y0 = ((player->y + player->y_half_size_of_screen) & (V_SIZE - 1)) >> Y_SHIFT;
		}

		update_y1 = (update_y1 + 1) & (number_of_y_lists - 1);
		hide_y1 = (hide_y1 + 1) & (number_of_y_lists - 1);

		// Check for update
		for (int y = update_y0; y != update_y1; y = (y + 1) & (number_of_y_lists - 1)) {
			obj = y_lists[y].first();
			while (obj) {
				if (!(obj->visibility & mask) && check_visibility(player, obj)) {
					obj->visibility |= mask;
					if (!NON_STATIC(obj->ID)) {
						if (!(obj->last_update & mask)) {
							player->put_object(obj);
							obj->last_update |= mask;
						}
					} else {
						player->put_object(obj);
						obj->send_hide &= ~mask;
						obj->last_update |= mask;
					}
				}
				obj = obj->next;
			}
		}
		// Check for hide
		for (int y = hide_y0; y != hide_y1; y = (y + 1) & (number_of_y_lists - 1)) {
			obj = y_lists[y].first();
			while (obj) {
				if (obj->visibility & mask && !check_visibility(player, obj)) {
					obj->visibility &= ~mask;
					if (NON_STATIC(obj->ID)) {
						player->put_object(obj);
						obj->send_hide |= mask;
					}
				}
				obj = obj->next;
			}
		}
	} else {
		// Check for update
		int update_y0 = ((player->y - player->y_half_size_of_screen) & (V_SIZE - 1)) >> Y_SHIFT;
		int update_y1 = ((player->y + player->y_half_size_of_screen) & (V_SIZE - 1)) >> Y_SHIFT;
		for (int y = update_y0; y != update_y1; y = (y + 1) & (number_of_y_lists - 1)) {
			obj = y_lists[y].first();
			while (obj) {
				if (check_visibility(player, obj)) {
					if (!NON_STATIC(obj->ID)) {
						if (!(obj->last_update & mask)) {
							player->put_object(obj);
							obj->last_update |= mask;
						}
					} else {
						if (!((obj->last_update | obj->visibility) & mask)) {
							player->put_object(obj);
							obj->send_hide &= ~mask;
							obj->last_update |= mask;
						}
					}
					obj->visibility |= mask;
				} else
					obj->visibility &= ~mask;
				obj = obj->next;
			}
		}
	}
	player->x_prev = player->x;
	player->y_prev = player->y;
	player->y_half_size_of_screen_prev = player->y_half_size_of_screen;

	// Checking inventories
	int send_position = IS_PAST(player->last_sent_position + 5000) ? 1 : 0;
	Player *p = current_players.first();
	while (p) {
		if (p != player)
		{
			if (check_visibility(p, player)) {
				// All's inventories to Me
				obj = p->inventory.first();
				while (obj) {
					if (!(obj->last_update & player->client_mask)) {
						player->put_object(obj);
						obj->last_update |= player->client_mask;
					}
					obj = obj->next;
				}
				// My inventory to All
				obj = player->inventory.first();
				while (obj) {
					if (!(obj->last_update & p->client_mask)) {
						p->put_object(obj);
						obj->last_update |= p->client_mask;
					}
					obj = obj->next;
				}
			} else if (send_position) {
				p->code_queue.put(Event(PLAYERS_POSITION, player));
				send_position = 2;
			}
		}
		p = p->next_alt;
	}
	if (send_position == 2)
		player->last_sent_position = SDL_GetTicks();
}

int World::check_visibility(Player *p1, Player *p2) {
	if (abs(getDistY(p1->y, p2->y)) <
		std::max(p1->y_half_size_of_screen, p2->y_half_size_of_screen) + PLAYERS_RADIUS)
		return 1;
	return 0;
}

void World::process_create_inventory(Player *player, Object *obj) {
	number_of_objects++;
	objects.append(obj);
	player->inventory.append(obj);
	obj->list = &(player->inventory);
	Player *p = current_players.first();
	while (p) {
		if (p != player) {
			if (check_visibility(p, player)) {
				p->put_object(obj);
				obj->last_update |= p->client_mask;
			}
		} else
			obj->last_update |= p->client_mask;
		p = p->next_alt;
	}
}

void World::process_update_inventory(Player *player, Object *obj) {
	obj->last_update = 0;
	Player *p = current_players.first();
	while (p) {
		if (p != player) {
			if (check_visibility(p, player)) {
				p->put_object(obj);
				obj->last_update |= p->client_mask;
			}
		} else
			obj->last_update |= p->client_mask;
		p = p->next_alt;
	}
}

/******************************************************************
				Player
******************************************************************/
Player::Player(Server *serv, XSocket &sock)
	: in_buffer(IN_BUFFER_SIZE), out_buffer(OUT_BUFFER_SIZE), object_queue(OUT_QUEUE_SIZE),
	  code_queue(OUT_QUEUE_SIZE) {
	server = serv;
	socket = sock;

	identificated = 0;
	client_version = 0;
	status = 0;
	game = 0;
	ID = 0;
	client_mask = 0;
	password = name = 0;
	prev_rating = 0;
	world = 0;
	x = y = y_half_size_of_screen = 0;
	x_prev = y_prev = y_half_size_of_screen_prev = 0;
	last_IO_operation = SDL_GetTicks() + 20 * 1000;
	last_sent_position = SDL_GetTicks();
	birth_time = SDL_GetTicks();
	//	current_sent_object = 0;
	time_to_remove = 0;
	next = prev = 0;
	list = 0;
	next_alt = prev_alt = 0;
	list_alt = 0;
}

Player::~Player() {
	if (name)
		delete name;
	if (password)
		delete password;
}

void Player::identification() {
	char string[256] = {0};
	static const char *request_str = "Vivat Sicher, Rock'n'Roll forever!!!";
	static const char *response_str = "Enter, my son, please...";
	static const char *kill_str = "I'm sorry, darling...";
	unsigned int len;
	if ((len = socket.receive(string, sizeof(string) - 1)) != 0) {
		if (!strcmp(string, request_str)) {
			identificated = 1;
			if (len > strlen(request_str) + 1)
				client_version = ((unsigned char *)string)[len - 1];
			strcpy(string, response_str);
			string[strlen(string) + 1] = SERVER_VERSION;
			socket.send(string, strlen(string) + 2);
			return;
		}
		// if (!strcmp(string, kill_str))
		// 	GlobalExit = 1;
	}
}

int Player::is_alive() {
	if (socket()) {
		if ((int)(SDL_GetTicks() - last_IO_operation) > 3000) {
			short size = 0;
			socket.send((const char *)&size, 2);
			last_IO_operation = SDL_GetTicks();
		}
		return 1;
	}
	if (!game)
		return 0;
	if (!time_to_remove) {
		time_to_remove = SDL_GetTicks() + WAITING_TO_REMOVE;
		MOUT1("Connection have been lost", ID);
		return 1;
	}
	return (int)(SDL_GetTicks() - time_to_remove) < 0 ? 1 : 0;
}

void Player::clear_object_queue(int keep_globals) {
	int number = object_queue.tell();
	for (; number > 0; number--) {
		Object *obj = object_queue.get();
		if (!obj)
			continue;
		if (keep_globals && !NON_GLOBAL_OBJECT(obj->ID)) {
			object_queue.put(obj);
			continue;
		}
		obj->in_queue &= ~client_mask;
		if (obj->send_delete & client_mask && (obj->send_delete &= ~client_mask) == 0) {
			if (NON_GLOBAL_OBJECT(obj->ID))
				world->delete_object(obj);
			else
				// if(obj -> ID != DIRECT_SENDING_OBJECT)
				if (obj->list)
				obj->list->remove(obj);
			delete obj;
		}
	}
}

int Player::receive() {
	if (!socket)
		return 0;
	int recv_size = in_buffer.receive(socket);
	if (recv_size) {
		last_IO_operation = SDL_GetTicks();
		//		IN_EVENTS_LOG1(Receive_Block,recv_size);
	}
	int code;
	while ((code = in_buffer.current_event()) != 0) {
		if (!(code & AUXILIARY_EVENT))
			code &= ~ECHO_EVENT;
		switch (code) {
		case CREATE_PERMANENT_OBJECT: {
			int obj_ID = in_buffer.get_dword();
			Object *obj = 0;
			if (NON_GLOBAL_OBJECT(obj_ID)) {
				if (!world) {
					SERVER_ERROR_NO_EXIT("Create permanent object before set world", 0);
					in_buffer.ignore_event();
					break;
				}
				obj = world->search_object(obj_ID);
			} else
				obj = game->global_objects.search(obj_ID);

			if (obj) {
				if (obj->send_delete) {
					if (NON_GLOBAL_OBJECT(obj->ID))
						world->delete_object(obj);
					else
						obj->list->remove(obj);
					obj->ID = 0;
				} else {
					obj->last_update |= client_mask;
					put_object(obj);
					DOUT1("Duplicated create", obj_ID);
					in_buffer.ignore_event();
					break;
				}
			}

			obj = new Object();
			obj->ID = obj_ID;
			obj->client_ID = ID;
			obj->time = in_buffer.get_dword();
			obj->x = in_buffer.get_short();
			obj->y = in_buffer.get_short();
			obj->radius = in_buffer.get_short();
			if (GET_OBJECT_TYPE(obj_ID) == NID_VANGER) {
				x = obj->x;
				y = obj->y;
				y_half_size_of_screen = in_buffer.get_byte() << 1;
				obj->body_size = in_buffer.event_size() - 16;
				obj->body = new unsigned char[obj->body_size];
				in_buffer.read(obj->body, obj->body_size);
				world->process_create(!(in_buffer.current_event() & ECHO_EVENT) ? this : 0, obj);
				world->process_set_position(this);
			} else {
				obj->body_size = in_buffer.event_size() - 15;
				obj->body = new unsigned char[obj->body_size];
				in_buffer.read(obj->body, obj->body_size);
				if (!PLAYERS_OBJECT(obj_ID))
					world->process_create(
						!(in_buffer.current_event() & ECHO_EVENT) ? this : 0, obj);
				else {
					if (NON_GLOBAL_OBJECT(obj_ID))
						world->process_create_inventory(this, obj);
					else
						game->process_create_globals(
							!(in_buffer.current_event() & ECHO_EVENT) ? this : 0, obj);
				}
			}
			IN_EVENTS_LOG1(CREATE_PERMANENT_OBJECT, obj->ID);
		} break;

		case DELETE_OBJECT: {
			int obj_ID = in_buffer.get_dword();
			Object *obj = 0;
			if (NON_GLOBAL_OBJECT(obj_ID)) {
				if (!world) {
					SERVER_ERROR_NO_EXIT("Delete object before set world", 0);
					in_buffer.ignore_event();
					break;
				}
				obj = world->search_object(obj_ID);
			} else
				obj = game->global_objects.search(obj_ID);
			if (!obj || obj->send_delete) {
				IN_EVENTS_LOG1(SKIP_DELETE_OBJECT, obj_ID);
				in_buffer.ignore_event();
				break;
			}
			obj->client_ID = ID;
			obj->time = in_buffer.get_dword();
			obj->death_body_size = in_buffer.event_size() - 9;
			if (obj->death_body_size > obj->body_size) {
				if (obj->body)
					delete obj->body;
				obj->body = new unsigned char[obj->death_body_size];
			}
			in_buffer.read(obj->body, obj->death_body_size);
			if (NON_GLOBAL_OBJECT(obj_ID))
				world->process_delete(obj);
			else
				game->process_delete_globals(obj);
			IN_EVENTS_LOG1(DELETE_OBJECT, obj->ID);
		} break;

		case UPDATE_OBJECT: {
			int obj_ID = in_buffer.get_dword();
			Object *obj = 0;
			if (NON_GLOBAL_OBJECT(obj_ID)) {
				if (!world) {
					SERVER_ERROR_NO_EXIT("Update object before set world", 0);
					in_buffer.ignore_event();
					break;
				}
				obj = world->search_object(obj_ID);
			} else
				obj = game->global_objects.search(obj_ID);
			if (!obj || obj->send_delete) {
				IN_EVENTS_LOG1(SKIP_UPDATE_OBJECT, obj_ID);
				in_buffer.ignore_event();
				break;
			}
			obj->client_ID = ID;
			obj->time = in_buffer.get_dword();
			obj->x = in_buffer.get_short();
			obj->y = in_buffer.get_short();
			if (GET_OBJECT_TYPE(obj_ID) == NID_VANGER) {
				x = obj->x;
				y = obj->y;
				y_half_size_of_screen = in_buffer.get_byte() << 1;
				int update_size = in_buffer.event_size() - 14;
				if (update_size > obj->body_size)
					SERVER_ERROR_NO_EXIT("Update body size is greater than create size", obj_ID);
				in_buffer.read(obj->body, update_size);
				world->process_update(!(in_buffer.current_event() & ECHO_EVENT) ? this : 0, obj);
				world->process_set_position(this);
			} else {
				int update_size = in_buffer.event_size() - 13;
				if (update_size > obj->body_size)
					SERVER_ERROR_NO_EXIT("Update body size is greater than create size", obj_ID);
				in_buffer.read(obj->body, update_size);
				if (!PLAYERS_OBJECT(obj_ID))
					world->process_update(
						!(in_buffer.current_event() & ECHO_EVENT) ? this : 0, obj);
				else {
					if (NON_GLOBAL_OBJECT(obj_ID))
						world->process_update_inventory(this, obj);
					else
						game->process_update_globals(
							!(in_buffer.current_event() & ECHO_EVENT) ? this : 0, obj);
				}
			}
			IN_EVENTS_LOG1(UPDATE_OBJECT, obj->ID);
		} break;

		case SET_POSITION:
			if (!world) {
				SERVER_ERROR_NO_EXIT("Set position before set world", 0);
				in_buffer.ignore_event();
				break;
			}
			x = in_buffer.get_short();
			y = in_buffer.get_short();
			y_half_size_of_screen = in_buffer.get_short();
			if (world)
				world->process_set_position(this);
			IN_EVENTS_LOG(SET_POSITION);
			break;

		case TOP_LIST_QUERY:
			server->get_top_list(out_buffer, in_buffer.get_byte());
			IN_EVENTS_LOG(TOP_LIST_QUERY);
			break;
		case GAMES_LIST_QUERY:
			code_queue.put(GAMES_LIST_RESPONSE);
			IN_EVENTS_LOG(GAMES_LIST_QUERY);
			break;

		case ATTACH_TO_GAME:
			if (game || ID)
				SERVER_ERROR_NO_EXIT("Player have already been attached to game", game->ID);
			if ((game = server->games.search(in_buffer.get_int())) == 0 ||
				(game->data.GameType == UNCONFIGURED && game->players.size()) ||
				game->used_players_IDs == 0x7fffffff)
				game = server->create_game();
			game->attach_player(this);
			game->check_global_objects(this);

			out_buffer.begin_event(ATTACH_TO_GAME_RESPONSE);
			out_buffer < game->ID < char(game->data.GameType != UNCONFIGURED ? 1 : 0) <
				(unsigned int)round((double)game->birth_time * (256. / 1000.)) < (unsigned char)ID;
			game->get_object_ID_offsets(out_buffer, ID);
			out_buffer.end_event();

			IN_EVENTS_LOG(ATTACH_TO_GAME);
			break;

		case RESTORE_CONNECTION: {
			Player *p;
			Game *game;
			if ((game = server->games.search(in_buffer.get_int())) != 0 &&
				(p = game->players.search(in_buffer.get_byte())) != 0) {
				if (p->socket())
					p->socket.close();
				p->socket = socket;
				p->time_to_remove = 0;
				MOUT1("Connection have been restored", p->ID);
				p->out_buffer.begin_event(RESTORE_CONNECTION_RESPONSE);
				p->out_buffer < char(1);
				p->out_buffer.end_event();
				IN_EVENTS_LOG(RESTORE_CONNECTION);
			} else {
				out_buffer.begin_event(RESTORE_CONNECTION_RESPONSE);
				out_buffer < char(0);
				out_buffer.end_event();
				IN_EVENTS_LOG(RESTORE_CONNECTION);
			}
		} break;

		case REGISTER_NAME: {
			if (name)
				delete name;
			int name_len = strlen(in_buffer(in_buffer.tell()));
			name = new char[name_len + 2];
			in_buffer > name;
			name[name_len + 1] = 0;
			int number = 0;
			Player *p = game->players.first();
			while (p) {
				if (p != this && p->name && !strcmp(p->name, name)) {
					name[name_len] = '0' + ++number;
					p = game->players.first();
				} else
					p = p->next;
			}
			if (password)
				delete password;
			password = new char[strlen(in_buffer(in_buffer.tell())) + 1];
			in_buffer > password;

			game->put_event_for_all(PLAYERS_NAME, this);

			IN_EVENTS_LOG(REGISTER_NAME);
			break;
		}

		case SET_WORLD: {
			if (world) {
				SERVER_ERROR_NO_EXIT("Duplicated set world", ID);
				in_buffer.ignore_event();
				break;
			}
			int world_ID = in_buffer.get_byte();
			int world_y_size = in_buffer.get_short();
			int world_status = 0;
			if ((world = game->worlds.search(world_ID)) == 0) {
				world = new World(world_ID, world_y_size);
				game->worlds.append(world);
				world_status = 1;
			} else if (world_y_size != world->V_SIZE)
				SERVER_ERROR_NO_EXIT("Incorrect world Y size", world_y_size * 100000 + world->V_SIZE);

			out_buffer.begin_event(SET_WORLD_RESPONSE);
			out_buffer < (unsigned char)world_ID;
			out_buffer < (unsigned char)world_status;
			out_buffer.end_event();

			world->attach_player(this);

			IN_EVENTS_LOG(SET_WORLD);
			OUT_EVENTS_LOG1(SET_WORLD_RESPONSE, world_status);
		} break;

		case LEAVE_WORLD:
			if (!world) {
				SERVER_ERROR_NO_EXIT("Leave world before set", 0);
				in_buffer.ignore_event();
				break;
			}
			world->detach_player(this);
			IN_EVENTS_LOG(STOP_QUERY);
			break;

		case SERVER_TIME_QUERY:
			out_buffer.begin_event(SERVER_TIME);
			out_buffer < (unsigned int)GLOBAL_CLOCK();
			out_buffer.end_event();
			IN_EVENTS_LOG(SERVER_TIME_QUERY);
			OUT_EVENTS_LOG(SERVER_TIME);
			break;

		case SET_GAME_DATA: {
			if (game->data.GameType != UNCONFIGURED) {
				DOUT1("Attempt to reassign game data", game->ID);
			}
			in_buffer > game->name;
			int size = in_buffer.event_size() - 1 - strlen(game->name) - 1;
			if (size != sizeof(ServerData))
				SERVER_ERROR_NO_EXIT("Incorrect Server Data", size);
			in_buffer.read((unsigned char *)&game->data, sizeof(ServerData));
			game->client_version = client_version;
			IN_EVENTS_LOG(SET_GAME_DATA);
		} break;

		case GET_GAME_DATA:
			code_queue.put(GAME_DATA_RESPONSE);
			IN_EVENTS_LOG(GET_GAME_DATA);
			break;

		case SET_PLAYER_DATA: {
			int size = in_buffer.event_size() - 1;
			if (size != sizeof(PlayerBody))
				SERVER_ERROR_NO_EXIT("Incorrect Player Body", size);
			static PlayerBody prev_body;
			prev_body = body;
			in_buffer.read((unsigned char *)&body, sizeof(PlayerBody));
			switch (game->data.GameType) {
			case VAN_WAR:
				if (body.kills != prev_body.kills || body.deaths != prev_body.deaths ||
					body.rating != prev_body.rating) {
					game->process_VAN_WAR_ratings();
					if (fabs(body.rating - prev_body.rating) > 0.01)
						code_queue.put(Event(PLAYERS_RATING, this));
				}
				break;
			case MECHOSOMA:
				if (body.MechosomaStat.MaxTransitTime) {
					game->process_MECHOSOMA_ratings();
					if (fabs(body.rating - prev_body.rating) > 0.01)
						code_queue.put(Event(PLAYERS_RATING, this));
				}
				break;
			case PASSEMBLOSS:
				if (body.PassemblossStat.TotalTime) {
					game->process_PASSEMBLOSS_ratings();
					if (fabs(body.rating - prev_body.rating) > 0.01)
						code_queue.put(Event(PLAYERS_RATING, this));
				}
				break;
			}
			game->put_event_for_all(PLAYERS_DATA, this);
			IN_EVENTS_LOG(SET_PLAYER_DATA);
		} break;

		case TOTAL_PLAYERS_DATA_QUERY:
			code_queue.put(TOTAL_LIST_OF_PLAYERS_DATA);
			IN_EVENTS_LOG(TOTAL_PLAYERS_DATA_QUERY);
			break;

		case DIRECT_SENDING: {
			unsigned int mask = in_buffer.get_dword();
			Object *obj = new Object();
			obj->ID = DIRECT_SENDING_OBJECT;
			obj->client_ID = ID;
			obj->time = GLOBAL_CLOCK();
			obj->body_size = in_buffer.event_size() - 5;
			obj->body = new unsigned char[obj->body_size];
			in_buffer.read(obj->body, obj->body_size);
			game->process_direct_sending(obj, mask);
			IN_EVENTS_LOG(DIRECT_SENDING);
		} break;

		case CLOSE_SOCKET:
			socket.close();
			time_to_remove = SDL_GetTicks();
			break;

		default:
			SERVER_ERROR_NO_EXIT("Incorrect event ID", in_buffer.current_event());
		}
		in_buffer.next_event();
	}
	return recv_size;
}

int Player::send() {
	if (!socket)
		return 0;
	Object *obj;
	int code, total_sent = 0;
	for (;;) {
		if (out_buffer.tell()) {
			int sent;
			total_sent += sent = out_buffer.send(socket);
			last_IO_operation = SDL_GetTicks();
			//			OUT_EVENTS_LOG1(Send_Block,sent);
			if (out_buffer.tell())
				return total_sent;
		}

		// if(!current_sent_object)
		//	current_sent_object = object_queue.get();
		// if(current_sent_object && current_sent_object -> time + LAG < GLOBAL_CLOCK())
		//	obj = current_sent_object;
		//	current_sent_object = 0;

		// Object's Queue
		if (!object_queue.empty()) {
			obj = object_queue.get();

			obj->in_queue &= ~client_mask;

			// Check for deleted objects
			if (!obj->ID) {
				if ((obj->send_delete &= ~client_mask) == 0)
					delete obj;
				continue;
			}

			if (obj->send_delete) { // !!!
				if (obj->ID != DIRECT_SENDING_OBJECT) {
					out_buffer.begin_event(DELETE_OBJECT);
					out_buffer < obj->ID < (unsigned char)obj->client_ID < obj->time;
					out_buffer.write(obj->body, obj->death_body_size);
					out_buffer.end_event();
					OUT_EVENTS_LOG1(DELETE_OBJECT, obj->ID);
					if ((obj->send_delete &= ~client_mask) == 0) {
						if (NON_GLOBAL_OBJECT(obj->ID))
							world->delete_object(obj);
						else
							obj->list->remove(obj);
						delete obj;
					}
				} else {
					out_buffer.begin_event(DIRECT_RECEIVING);
					out_buffer < (unsigned char)obj->client_ID;
					out_buffer.write(obj->body, obj->body_size);
					out_buffer.end_event();
					OUT_EVENTS_LOG(DIRECT_RECEIVING);
					if ((obj->send_delete &= ~client_mask) == 0)
						delete obj;
				}
				continue;
			}

			if (!(obj->send_hide & client_mask)) {
				out_buffer.begin_event(UPDATE_OBJECT);
				out_buffer < obj->ID < (unsigned char)obj->client_ID < obj->time < (short)obj->x <
					(short)obj->y;
				out_buffer.write(obj->body, obj->body_size);
				out_buffer.end_event();
				OUT_EVENTS_LOG1(UPDATE_OBJECT, obj->ID);
				continue;
			} else {
				out_buffer.begin_event(HIDE_OBJECT);
				out_buffer < obj->ID;
				out_buffer.end_event();
				OUT_EVENTS_LOG1(HIDE_OBJECT, obj->ID);
				obj->send_hide &= ~client_mask;
				continue;
			}
		}

		// Code Queue
		if (!code_queue.empty()) {
			Event event = code_queue.get();
			code = event.code;
			Player *player = event.pointer;
			switch (code) {
			case GAMES_LIST_RESPONSE:
				server->get_games_list(out_buffer, client_version);
				OUT_EVENTS_LOG(GAMES_LIST_RESPONSE);
				break;

			case TOTAL_LIST_OF_PLAYERS_DATA:
				game->get_total_list_of_players_data(out_buffer);
				OUT_EVENTS_LOG(TOTAL_LIST_OF_PLAYERS_DATA);
				break;

			case GAME_DATA_RESPONSE:
				out_buffer.begin_event(GAME_DATA_RESPONSE);
				out_buffer < game->name < char(0);
				out_buffer.write(&game->data, sizeof(ServerData));
				out_buffer.end_event();
				OUT_EVENTS_LOG(GAME_DATA_RESPONSE);
				break;

			case PLAYERS_NAME:
				out_buffer.begin_event(PLAYERS_NAME);
				out_buffer < (unsigned char)player->ID < player->name < char(0);
				out_buffer.end_event();
				OUT_EVENTS_LOG(PLAYERS_NAME);
				break;
			case PLAYERS_POSITION:
				out_buffer.begin_event(PLAYERS_POSITION);
				out_buffer < (unsigned char)player->ID < short(player->x) < short(player->y);
				out_buffer.end_event();
				OUT_EVENTS_LOG(PLAYERS_POSITION);
				break;
			case PLAYERS_WORLD:
				out_buffer.begin_event(PLAYERS_WORLD);
				out_buffer < (unsigned char)player->ID <
					(unsigned char)(player->world ? player->world->ID : 0);
				out_buffer.end_event();
				OUT_EVENTS_LOG(PLAYERS_WORLD);
				break;
			case PLAYERS_DATA:
				out_buffer.begin_event(PLAYERS_DATA);
				out_buffer < (unsigned char)player->ID;
				out_buffer.write(&(player->body), sizeof(PlayerBody));
				out_buffer.end_event();
				OUT_EVENTS_LOG(PLAYERS_DATA);
				break;
			case PLAYERS_RATING:
				out_buffer.begin_event(PLAYERS_RATING);
				out_buffer < (unsigned char)player->ID < (float)player->body.rating;
				out_buffer.end_event();
				OUT_EVENTS_LOG(PLAYERS_RATING);
				break;
			case PLAYERS_STATUS:
				out_buffer.begin_event(PLAYERS_STATUS);
				out_buffer < (unsigned char)player->ID < (unsigned char)player->status;
				out_buffer.end_event();
				OUT_EVENTS_LOG(PLAYERS_STATUS);
				break;

			default:
				SERVER_ERROR_NO_EXIT("Incorrect event ID in code queue", code);
			}
			continue;
		}

		return total_sent;
	}
}

void Player::put_object(Object *object) {
	if (!(object->in_queue & client_mask)) {
		object->in_queue |= client_mask;
		object_queue.put(object);
	}
}

/******************************************************************
				Object
******************************************************************/
Object::Object() {
	ID = 0;
	time = 0;
	x = y = radius = 0;

	visibility = 0;
	last_update = 0;
	in_queue = 0;
	send_hide = 0;
	send_delete = 0;

	death_body_size = body_size = 0;
	body = 0;

	next = prev = 0;
	list = 0;
	next_alt = prev_alt = 0;
	list_alt = 0;
}

Object::~Object() {
	if (body)
		delete body;
}

/***********************************************************************
				Input Event Buffer
***********************************************************************/
InputEventBuffer::InputEventBuffer(unsigned int size): XBuffer(size) {
	reset();
}

void InputEventBuffer::reset() {
	next_event_pointer = 0;
	filled_size = 0;
	offset = 0;
}

int InputEventBuffer::receive(XSocket &sock) {
	if (next_event_pointer != tell()) {
		SERVER_ERROR_NO_EXIT("Incorrect events reading", 0);
		ignore_event();
		return 0;
	}
	if (next_event_pointer) {
		if (filled_size != next_event_pointer)
			memmove(address(), address() + next_event_pointer, filled_size - next_event_pointer);
		filled_size -= next_event_pointer;
		offset = next_event_pointer = 0;
	}

	int add_size = sock.receive((char *)address() + filled_size, length() - filled_size);
	filled_size += add_size;
	next_event();
	return add_size;
}

int InputEventBuffer::next_event() {
	event_ID = 0;

	if (next_event_pointer + 2 > filled_size)
		return 0;
	if (next_event_pointer != tell()) {
		SERVER_ERROR_NO_EXIT("Incorrect events reading", 0);
		ignore_event();
		return 0;
	}

	size_of_event = get_short();
	unsigned int new_pointer = next_event_pointer + size_of_event + 2;
	if (new_pointer > filled_size) {
		set(next_event_pointer);
		return 0;
	}
	next_event_pointer = new_pointer;

	return (event_ID = get_byte());
}

void InputEventBuffer::ignore_event() {
	event_ID = 0;
	set(next_event_pointer);
}

/***********************************************************************
				Output Event Buffer
***********************************************************************/
OutputEventBuffer::OutputEventBuffer(unsigned int size): XBuffer(size) {
	pointer_to_size_of_event = -1;
}

int OutputEventBuffer::send(XSocket &sock) {
	if (pointer_to_size_of_event != -1) {
		SERVER_ERROR_NO_EXIT("There wasn't the end of the event", 0);
		return 0;
	}
	unsigned int sent = sock.send(buf, tell());
	if (sent == tell())
		init();
	else {
		memmove(buf, buf + sent, tell() - sent);
		offset -= sent;
	}
	return sent;
}

void OutputEventBuffer::begin_event(int event_ID) {
	if (pointer_to_size_of_event != -1) {
		SERVER_ERROR_NO_EXIT("There wasn't the end of the event", 0);
		return;
	}
	pointer_to_size_of_event = offset;
	*this < short(0) < (unsigned char)event_ID;
}

void OutputEventBuffer::end_event() {
	if (pointer_to_size_of_event == -1) {
		SERVER_ERROR_NO_EXIT("There wasn't the begining of the event", 0);
		return;
	}
	int off = tell();
	set(pointer_to_size_of_event);
	*this < short(off - pointer_to_size_of_event - 2);
	set(off);
	pointer_to_size_of_event = -1;
}

/******************************************************************
				Server
******************************************************************/
Server::Server(int main_port, int broadcast_port, int time_to_live) {
	if (!main_socket.listen(main_port)) {
		XBuffer err;
		err < "Vangers Server is already running or TCP/IP port " <= main_port <
			" is used by another application";
		ErrH.Abort(err.GetBuf());
		// SERVER_ERROR("Unable to create Server",main_port);
	}
	std::cout << "Main TCP/IP port: " << main_port << std::endl;

	load_rating_list("Rating.lst");

	transferring = 0;
	next_broadcast = 0;
	games_IDs_counter = 0;
	Server::time_to_live = time_to_live;
	time_to_destroy = 0;

	if (StatLogging)
		stat_log.open("VangersServer.log", XS_OUT);
	for (int i = 0; i < NUMBER_MP_GAMES; i++)
		n_games[i] = n_players_max[i] = n_players_sum[i] = playing_time_max[i] =
			playing_time_sum[i] = 0;
}

Server::~Server() {
	save_rating_list("Rating.lst");
	RatingData *p;
	while ((p = rating_list.first()) != 0) {
		rating_list.remove(p);
		delete p;
	}

	clear();
}

void Server::clear() {
	Game *g;
	while ((g = games.first()) != 0) {
		games.remove(g);
		delete g;
	}
	Player *p;
	while ((p = clients.first()) != 0) {
		clients.remove(p);
		delete p;
	}
	DOUT("Clear Server");
}

Game *Server::create_game() {
	Game *game = new Game(++games_IDs_counter);
	games.append(game);
	DOUT1("Game created", games_IDs_counter);
	return game;
}

int Server::check_new_clients() {
	if (clients.size() >= 256) {
		return 0;
	}
	if (!main_socket)
		return 0;

	XSocket &&sock = main_socket.accept();
	if (!sock)
		return 0;

	Player *player = new Player(this, sock);
	clients.append(player);
	if (clients.size() == 256) {
		SERVER_ERROR_NO_EXIT("DDOS", 0);
	}
	DOUT("Client attached");

	return 1;
}

int Server::clients_quant() {
	int work_log = 0;
	Player *p = clients.first();
	while (p) {
		if (p->identificated) {
			work_log += p->receive();
			work_log += p->send();
		} else
			p->identification();

		if (!p->is_alive()) {
			Player *p_next = p->next;
			//			DOUT("Client removed");
			clients.remove(p);
			delete p;
			p = p_next;
		} else
			p = p->next;
	}
	return work_log;
}

void Server::consoleReport(int players) {
	static int g = -1;
	static int p = -1;
	static int c = -1;

	if (g == games.size() && p == players && c == clients.size())
		return;

	g = games.size();
	p = players;
	c = clients.size();

	std::cout << "Games: " << games.size() << " "
			  << "Players: " << players << " "
			  << "Clients: " << clients.size() << " "
			  << "\n";
}

int Server::quant() {
	// fout < "Quant: " <= frame < "\t" <= SDL_GetTicks() < "\n";
	if (next_broadcast < SDL_GetTicks()) {
		next_broadcast = SDL_GetTicks() + 1000;
		int n_players = 0;
		Game *g = games.first();
		while (g) {
			n_players += g->players.size();
			g = g->next;
		}
		consoleReport(n_players);
		if (time_to_live && !(transferring | games.size() | n_players | clients.size())) {
			if (!time_to_destroy)
				time_to_destroy = SDL_GetTicks() + time_to_live * 1000;
		} else
			time_to_destroy = 0;
		if (time_to_destroy && IS_PAST(time_to_destroy))
			GlobalExit = 1;
		transferring = 0;
		// report();
	}
	int transf = check_new_clients() + clients_quant() + games_quant();
	transferring += transf;
	return transf;
}

int Server::games_quant() {
	int work_log = 0;
	Game *g = games.first();
	while (g) {
		work_log += g->quant();
		if (!g->players.size() && !leave_empty_games) {
			analyse_statistics(g);
			Game *g_next = g->next;
			games.remove(g);
			delete g;
			g = g_next;
			continue;
		}
		g = g->next;
	}
	return work_log;
}

void Server::analyse_statistics(Game *g) {
	int playing_time = round(double(TIME_INTERVAL(g->birth_time)) / (60 * 1000));
	int n_players = g->removed_players.size();
	if (!n_players || playing_time < 5)
		return;

	int type = g->data.GameType;
	n_games[type]++;
	if (n_players_max[type] < n_players)
		n_players_max[type] = n_players;
	n_players_sum[type] += n_players;
	if (playing_time_max[type] < playing_time)
		playing_time_max[type] = playing_time;
	playing_time_sum[type] += playing_time;

	std::cout << "\nGames' Statistics (maximum/average):\n";
	for (int i = 0; i < NUMBER_MP_GAMES; i++) {
		if (!n_games[i])
			continue;
		std::cout << (char *)MP_GAMES_NAMES[i] << ": " << n_games[i]
				  << "; Players: " << n_players_max[i] << "/"
				  << round(double(n_players_sum[i]) / n_games[i])
				  << "; Time of game: " << playing_time_max[i] << "/"
				  << round(double(playing_time_sum[i]) / n_games[i]) << " min.\n";
	}

	std::cout << "Last game: ";
	time_t aclock;
	time(&aclock);
	struct tm *newtime = localtime(&aclock);
	char *strtime = asctime(newtime);
	strtime[strlen(strtime) - 1] = 0;
	std::cout << (char *)MP_GAMES_NAMES[type] << "; " << strtime << "; Players: " << n_players
			  << "; Time of game: " << playing_time << " min.\n\n";
	if (StatLogging) {
		stat_log < strtime < "; " < (char *)MP_GAMES_NAMES[type] < "; Players: " <= n_players <
			"; Time of game: " <= playing_time < " min.\n";
		Player *p = g->removed_players.first();
		while (p) {
			int IP = p->socket.addr.host;
			if (IP)
				stat_log < "IP: " <= (IP & 0xff) < "." <= ((IP >> 8) & 0xff) < "." <=
					((IP >> 16) & 0xff) < "." <= ((IP >> 24) & 0xff) < "; Time of Existence: " <=
					round(double(TIME_INTERVAL(p->birth_time)) / (60 * 1000)) < " min.; Rating: " <=
					round(p->body.rating) < "\n";
			p = p->next;
		}
		stat_log < "\n";
	}
}

void Server::get_games_list(OutputEventBuffer &out_buffer, int client_version) {
	int num = 0;
	Game *g = games.first();
	while (g) {
		if (g->data.GameType != UNCONFIGURED && g->used_players_IDs != 0x7fffffff &&
			g->client_version == client_version)
			num++;
		g = g->next;
	}
	out_buffer.begin_event(GAMES_LIST_RESPONSE);
	out_buffer < (unsigned char)num;
	g = games.first();
	while (g) {
		if (g->data.GameType != UNCONFIGURED && g->used_players_IDs != 0x7fffffff &&
			g->client_version == client_version) {
			out_buffer < g->ID < g->name < ": " <= g->players.size() < " " <
				(g->data.GameType == VAN_WAR ? "V" : (g->data.GameType == MECHOSOMA ? "M" : "P"));

			int t = (SDL_GetTicks() - g->birth_time) / 1000;
			int ts = t % 60;
			t /= 60;
			int tm = t % 60;
			t /= 60;
			int th = t % 24;
			out_buffer < " " <= th < ":" <= tm < ":" <= ts < char(0);
		}
		g = g->next;
	}
	out_buffer.end_event();
}

void Server::report() {
	std::cout << "\n\nReport's time: " << SECONDS() << "\n";
	Player *p;
	World *w;
	Game *s = games.first();
	while (s) {
		std::cout << "Game " << (unsigned char)(s->ID) << ": ";
		if (!(s->players.size() | s->worlds.size())) {
			std::cout << "Empty\n\n";
			s = s->next;
			continue;
		}
		std::cout << "\nPlayers: " << s->players.size() << "\tWorlds: " << s->worlds.size()
				  << "\tglobal objects: " << s->global_objects.size() << "\n";
		p = s->players.first();
		while (p) {
			std::cout << "Player: " << p->ID << "\tsocket: " << p->socket()
					  << "\tworld: " << (p->world ? p->world->ID : 0)
					  << "\tinventory: " << p->inventory.size() << "\t " << p->name << "\n";
			p = p->next;
		}

		w = s->worlds.first();
		while (w) {
			std::cout << "World: " << w->ID << "\ttotal objects: " << w->number_of_objects << "\n";
			w = w->next;
		}
		std::cout << "\n";
		s = s->next;
	}
}

/******************************************************************
				Rating List
******************************************************************/
RatingData::RatingData(char *_name, char *_password, int _MP_game, float _rating) {
	name = new char[strlen(_name) + 1];
	strcpy(name, _name);
	password = new char[strlen(_password) + 1];
	strcpy(password, _password);
	MP_game = _MP_game;
	rating = _rating;
	next = prev = 0;
	list = 0;
}

RatingData::~RatingData() {
	delete name;
	delete password;
}

void Server::load_rating_list(const char *fname) {
	XStream ff(0);
	if (!ff.open(fname, XS_IN))
		return;
	XBuffer buf(ff.size());
	ff.read(buf.address(), ff.size());
	char name[257];
	char password[257];
	int MP_game;
	float rating;
	RatingData *p;
	int num_data;
	buf > num_data;
	for (int i = 0; i < num_data; i++) {
		buf > name > password > MP_game > rating;
		p = new RatingData(name, password, MP_game, rating);
		rating_list.append(p);
	}

	for (int i = 0; i < NUMBER_MP_GAMES; i++)
		rating_threshoulds[i] = (float)-1e+10;
}

void Server::save_rating_list(const char *name) {
	XStream ff(name, XS_OUT);
	ff < rating_list.size();
	RatingData *p = rating_list.first();
	//	fout < "\n\nRating:\n";
	while (p) {
		ff < p->name < char(0) < p->password < char(0) < p->MP_game < p->rating;
		//		fout < p -> name < "\t" < p -> password  < "\t" <= p -> MP_game < "\t" <= p ->
		//rating < "\n";
		p = p->next;
	}
	//	fout < "\n";
}

RatingData *Server::search_rating_data(char *player_name, char *player_password, int MP_game) {
	RatingData *p = rating_list.first();
	while (p) {
		if (!strcmp(player_name, p->name) && !strcmp(player_password, p->password) &&
			MP_game == p->MP_game)
			return p;
		p = p->next;
	}
	return p;
}

void Server::add_rating_data(Player *player, int MP_game) {
	RatingData *p = search_rating_data(player->name, player->password, MP_game);

	if (fabs(player->body.rating - player->prev_rating) < 0.01 &&
		(p || player->body.rating < rating_threshoulds[MP_game]))
		return;

	if (p)
		rating_list.remove(p);
	else
		p = new RatingData(player->name, player->password, MP_game, 0);

	p->rating += player->body.rating - player->prev_rating;
	float rating = p->rating;
	int counter = 0;
	RatingData *pp = rating_list.first();
	while (pp && (pp->MP_game < MP_game || (pp->MP_game == MP_game && pp->rating > rating))) {
		if (pp->MP_game == MP_game)
			if (counter++ > 20) {
				rating_threshoulds[MP_game] = pp->rating;
				delete p;
				return;
			}
		pp = pp->next;
	}
	rating_list.insert(pp, p);
	player->prev_rating = player->body.rating;
}

void Server::get_top_list(OutputEventBuffer &out_buffer, int MP_game) {
	int num = 0;
	RatingData *p = rating_list.first();
	while (p) {
		if (p->MP_game == MP_game)
			num++;
		p = p->next;
	}
	if (num > 10)
		num = 10;
	out_buffer.begin_event(TOP_LIST_RESPONSE);
	out_buffer < (unsigned char)MP_game < (unsigned char)num;
	p = rating_list.first();
	while (p && num) {
		if (p->MP_game == MP_game) {
			out_buffer < p->name < char(0) < p->rating;
			num--;
		}
		p = p->next;
	}
	out_buffer.end_event();
}
