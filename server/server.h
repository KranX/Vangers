#include "../lib/xtool/xglobal.h"
#include "../src/multiplayer.h"
#include "xsocket.h"

#define DIRECT_SENDING_OBJECT 1

#define IN_BUFFER_SIZE 128000
#define OUT_BUFFER_SIZE 128000
#define OUT_QUEUE_SIZE (1 << 10)
#define IN_BUFFER_META_SIZE 4000
#define OUT_BUFFER_META_SIZE 20000
#define Y_SHIFT 2
#define PLAYERS_RADIUS 128
#define WAITING_TO_REMOVE (60 * 1000)

/*******************************************************************************
			Structures
*******************************************************************************/
struct Server;
struct Game;
struct Player;
struct World;

struct Event {
	int code;
	Player *pointer;

	Event() {
		code = 0;
		pointer = 0;
	}
	Event(int cd) {
		code = cd;
		pointer = 0;
	}
	Event(int cd, Player *ptr) {
		code = cd;
		pointer = ptr;
	}
	int operator==(Event &ev) {
		return code == ev.code && pointer == ev.pointer;
	}
	int operator!=(Event &ev) {
		return code != ev.code || pointer != ev.pointer;
	}
};

class OutputEventBuffer: public XBuffer {
	unsigned int pointer_to_size_of_event;

  public:
	OutputEventBuffer(unsigned int size);

	int send(XSocket &sock);
	void begin_event(int event_ID);
	void end_event();
};

class InputEventBuffer: public XBuffer {
	int event_ID;
	int size_of_event;

	unsigned int next_event_pointer;
	unsigned int filled_size;

  public:
	InputEventBuffer(unsigned int size);
	void reset();

	int receive(XSocket &sock);
	int current_event() {
		return event_ID;
	}
	int event_size() {
		return size_of_event;
	}
	int next_event();
	void ignore_event();

	int get_byte() {
		unsigned char t;
		*this > t;
		return (int)t;
	}
	int get_short() {
		short t;
		*this > t;
		return (int)t;
	}
	int get_word() {
		unsigned short t;
		*this > t;
		return (int)t;
	}
	int get_int() {
		int t;
		*this > t;
		return (int)t;
	}
	unsigned int get_dword() {
		unsigned int t;
		*this > t;
		return (unsigned int)t;
	}
	double get_float() {
		float t;
		*this > t;
		return (double)t;
	}
};

struct Object {
	int ID;
	int client_ID;
	unsigned int visibility;
	unsigned int last_update;
	unsigned int in_queue;
	unsigned int send_hide;
	unsigned int send_delete;
	int time;
	int x, y, radius;

	int body_size;
	int death_body_size;
	unsigned char *body;

	Object *next;
	Object *prev;
	XTList<Object> *list;

	Object *next_alt;
	Object *prev_alt;
	XTListAlt<Object> *list_alt;

	Object();
	~Object();
};

struct World {
	int ID;
	int V_SIZE;
	int number_of_objects;
	int number_of_y_lists;
	XTList<Object> *y_lists;
	XTListAlt<Object> objects;

	XTListAlt<Player> current_players;

	World *next;
	World *prev;
	XTList<World> *list;

	World(int ID, int V_SIZE);
	~World();

	void attach_player(Player *player);
	void detach_player(Player *player);

	Object *search_object(int ID);

	void add_object(Object *obj);
	void move_object(Object *obj);
	void delete_object(Object *obj);

	int getDistY(int v0, int v1);
	int check_visibility(Player *player, Object *object);
	int check_visibility(Player *p1, Player *p2);
	void process_create(Player *player, Object *obj);
	void process_update(Player *player, Object *object);
	void process_delete(Object *obj);
	void process_set_position(Player *player);
	void process_create_inventory(Player *player, Object *obj);
	void process_update_inventory(Player *player, Object *obj);
};

struct Player {
	int ID;
	int identificated;
	int client_version;
	int client_mask;
	int status;
	char *name;
	char *password;

	Server *server;
	Game *game;
	World *world;
	int x, y;
	int y_half_size_of_screen;
	int x_prev, y_prev;
	int y_half_size_of_screen_prev;

	PlayerBody body;
	float prev_rating;

	XTList<Object> inventory;

	// Object* current_sent_object;
	unsigned int birth_time;
	unsigned int last_sent_position;
	unsigned int last_IO_operation;
	unsigned int time_to_remove;
	int total_sent;
	int total_received;
	XSocket socket;
	InputEventBuffer in_buffer;
	OutputEventBuffer out_buffer;
	XQueue<Object *> object_queue;
	XQueue<Event> code_queue;

	Player *next;
	Player *prev;
	XTList<Player> *list;

	Player *next_alt;
	Player *prev_alt;
	XTListAlt<Player> *list_alt;

	Player(Server *server, XSocket &sock);
	~Player();

	void identification();
	int is_alive();
	int receive();
	int send();
	void put_object(Object *object);
	void clear_object_queue(int keep_globals);
};

struct Game {
	int ID;
	int client_version;
	unsigned int used_players_IDs;
	XTList<Player> players;
	XTList<Player> removed_players;
	XTList<World> worlds;
	XTList<Object> global_objects;

	unsigned int birth_time;
	char name[128];
	ServerData data;

	Game *next;
	Game *prev;
	XTList<Game> *list;

	Game(int ID);
	~Game();

	int attach_player(Player *player);
	void detach_player(Player *player);

	int quant();

	void get_object_ID_offsets(OutputEventBuffer &out_buffer, int client_ID);
	void put_event_for_all(int code, Player *player = 0);
	void process_direct_sending(Object *obj, unsigned int mask);
	void get_total_list_of_players_data(OutputEventBuffer &out_buffer);

	void check_global_objects(Player *player);
	void process_create_globals(Player *player, Object *obj);
	void process_update_globals(Player *player, Object *obj);
	void process_delete_globals(Object *obj);

	void process_VAN_WAR_ratings();
	void process_MECHOSOMA_ratings();
	void process_PASSEMBLOSS_ratings();
	void save_result();
	void load_result(Server *server, char *name);
};

struct RatingData {
	char *name;
	char *password;
	int MP_game;
	float rating;

	RatingData(char *_name, char *_password, int MP_game, float rating);
	~RatingData();

	RatingData *next;
	RatingData *prev;
	XTList<RatingData> *list;
};

struct Server {
	int games_IDs_counter;
	XTList<Game> games;
	XTList<Player> clients;
	float rating_threshoulds[NUMBER_MP_GAMES];
	XTList<RatingData> rating_list;

	XSocket main_socket;

	int transferring;
	unsigned int time_to_live;
	unsigned int time_to_destroy;
	int next_broadcast;

	int n_games[NUMBER_MP_GAMES];
	int n_players_max[NUMBER_MP_GAMES], n_players_sum[NUMBER_MP_GAMES];
	int playing_time_max[NUMBER_MP_GAMES], playing_time_sum[NUMBER_MP_GAMES];

	Server(int main_port, int broadcast_port, int time_to_live);
	~Server();
	void clear();

	Game *create_game();
	int quant();
	int check_new_clients();
	int clients_quant();
	int games_quant();
	void get_games_list(OutputEventBuffer &out_buffer, int client_version);
	void analyse_statistics(Game *g);
	void report();

	void consoleReport(int players);
	void load_rating_list(const char *name);
	void save_rating_list(const char *name);
	RatingData *search_rating_data(char *player_name, char *player_password, int MP_game);
	void add_rating_data(Player *player, int MP_game);
	void get_top_list(OutputEventBuffer &out_buffer, int MP_game);
};

/*******************************************************************************
			Defines
*******************************************************************************/
#define GLOBAL_CLOCK() (round(SDL_GetTicks() * (256. / 1000)))
#define SECONDS() (round(SDL_GetTicks() * (1. / 1000)))
#define SERVER_ERROR(str, code) ErrH.Abort(str, XERR_USER, code)
#define SERVER_ERROR_NO_EXIT(str, code) std::cout<<"Error: "<<str<<" code:"<<XERR_USER<<" val:"<<code<<std::endl

#define START_TIMER(interval) unsigned int _end_time_ = SDL_GetTicks() + interval;
#define CHECK_TIMER() ((int)(SDL_GetTicks() - _end_time_) < 0)
#define IS_FUTURE(time) ((int)((time)-SDL_GetTicks()) > 0)
#define IS_PAST(time) ((int)(SDL_GetTicks() - (time)) > 0)
#define TIME_INTERVAL(time) ((int)(SDL_GetTicks() - (time)))
