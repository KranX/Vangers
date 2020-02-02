#ifndef __NETWORK_H__
#define __NETWORK_H__
#include "zmod_client.h"

#include "xsocket.h"
#include "multiplayer.h"

#define BROADCAST_PORT		1992
#define DEFAULT_SERVER_PORT	2197

extern double zMod_cycle;
extern double zMod_flood_level_delta;

/*****************************************************************
			Механизм поиска сереров
*****************************************************************/
struct ServerFindChain {
	char* domain_name;
	int IP;
	int port;
	int game_ID;
	int configured;
	char name[128];
	ServerData data;
	ServerFindChain* next;
	ServerFindChain* prev;
	XTList<ServerFindChain>* list;

	ServerFindChain(int IP = 0,int port = 0,char* domain_name = 0,int game_ID = 0,char* game_name = 0);
	~ServerFindChain();
	void connect(XSocket &target);
	};

struct ServersList : XTList<ServerFindChain> {
	char domain_name[257];

	void clear_states();
	int talk_to_server(int IP,int port,char* domain_name,int only_new_game = 0);
	int find_servers(int bc_port = BROADCAST_PORT);
	int find_servers_in_the_internet(char* host_name,int host_port = DEFAULT_SERVER_PORT);
};

/*******************************************************************************
			Players manipulation tools
*******************************************************************************/
const int MAX_ACTIVE_SLOT = 8;
struct PlayerData {
	int client_ID;
	int x,y;
	int world;
	char* name;
	int status;
	int GamingFlag;
	PlayerData* next;
	PlayerData* prev;
	XTList<PlayerData>* list;

	int CreatePlayerFlag;
	void* uvsPoint;

	int SlotNetID[MAX_ACTIVE_SLOT];
	int SlotStuffNetID[MAX_ACTIVE_SLOT];
	int SlotFireCount[MAX_ACTIVE_SLOT];

	PlayerBody body;

		PlayerData(int client_id);
		~PlayerData(){ if(name) delete name; }
	};

struct PlayersList : XTList<PlayerData> {
	int NumGamingPeople;
		
		PlayersList(){ NumGamingPeople = 0; }

	PlayerData* find(int client_id);
	void single_parsing(int event_ID);
	void reset_states();
	void delete_old_players();
	void parsing_names_query();
	void parsing_total_body_query();
	unsigned int get_team_mask(int color);
	};

/*******************************************************************************
			Chat Tool
*******************************************************************************/
// send message modes
enum {
	MESSAGE_FOR_ALL,
	MESSAGE_FOR_TEAM,
	MESSAGE_FOR_PLAYER
	};

// teams...
enum {
	TEAM_GREEN,
	TEAM_ORANGE,
	TEAM_BLUE,
	TEAM_YELLOW
	};

struct MessageElement : XListElement {
	char* message;
	int color;
	int time; //zmod
	MessageElement(const char* player_name, char* msg,int color = 0);
	~MessageElement(){ delete message; }
	};

struct MessageDispatcher : XList {
	int max_number_of_messages;
	int new_messages; //zmod
	MessageDispatcher(int max_num_messages = 15) : XList() { max_number_of_messages = max_num_messages; }
	MessageElement* first(){ return (MessageElement*)fPtr; }
	MessageElement* last(){ return (MessageElement*)lPtr; }
	void send(char* message,int mode,int parameter);
	void receive();
	};

/*******************************************************************************
			Top List
*******************************************************************************/
struct RatingData {
	char* name;
	float rating;

		RatingData(char* _name,float rating);
		~RatingData();

	RatingData* next;
	RatingData* prev;
	XTList<RatingData>* list;
};

struct TopList : XTList<RatingData> {
	TopList(int MP_game);
	~TopList();
};

/*******************************************************************************
			Events Dispatchers
*******************************************************************************/
extern XSocket main_socket;

class OutputEventBuffer : public XBuffer {
	short n_event;
	int pointer_to_the_first_event;
	int pointer_to_the_last_event;
	int pointer_to_size_of_event;

	int full_event_size(int index);

public:

	OutputEventBuffer(unsigned int size);
	void clear();

	void create_permanent_object(int ID,int x,int y,int radius,int flags = 0);
	void update_object(int ID,int x,int y,int flags = 0);
	void delete_object(int ID);

	void begin_create_z_object(int Type, int ID);

	void begin_server_data();
	void begin_direct_send(unsigned int mask);
	void begin_event(int event_ID);

	void end_body();

	void register_name(char* name, char* password);
	void set_world(int world,int y_size_of_world);
	void set_position(int x,int y,int y_half_size_of_screen);
	void set_player_body(PlayerBody& body);


	int number_of_events(){ return n_event; }
	int send(int system_send = 0,XSocket& sock = main_socket); // returns number of sent bytes
	int send_simple_query(int event,XSocket& sock = main_socket); // Sends one-byte events
	int send_simple_query(int event, unsigned char data,XSocket& sock = main_socket); // Sends two-byte events
	int send_simple_query(int event, unsigned char data1, unsigned char data2,XSocket& sock = main_socket); // Sends three-byte events
	};

class InputEventBuffer : public XBuffer {
	unsigned char event_ID,client_ID,world;
	unsigned short event_size,body_size;
	unsigned short x,y,radius;
	int time,object_ID;

	unsigned int next_event_pointer;
	unsigned int filled_size;

public:
		InputEventBuffer(unsigned int size);
		void reset();

	int receive(XSocket& sock = main_socket,int dont_free = 0); // 0 if there isn't any enents,  first EventID othewise
	int receive_waiting_for_event(int event, XSocket& sock = main_socket, int skip_if_aint = 0); // Waits for event for a waiting_delay and moves it ot the begining of buffer. All others events will be saved

	int current_event(){ return event_ID; } // 0 if there isn't any enents,  EventID othewise
	int current_ID(){ return object_ID; }
	int current_creator(){ return client_ID; }
	int current_time(){ return time; }
	int current_x(){ return x; }
	int current_y(){ return y; }
	int current_radius(){ return radius; }
	int current_body_size(){ return body_size; }
	// 1. If there isn't such a datum in the event (x,y for delete_object), no error will be generated
	// 2. To read body use > or read() of XBuffer.
	void ignore_event(); // Skip body, if any
	int next_event(); // 0 if there isn't any enents,  EventID othewise

	int get_byte(){ unsigned char t; *this > t; return (int)t; }
	int get_short(){ short t; *this > t; return (int)t; }
	double get_float(){ float t; *this > t; return (double)t; }
	int get_word(){ unsigned short t; *this > t; return (int)t; }
	int get_int(){ int t; *this > t; return (int)t; }
	unsigned int get_dword(){ unsigned int t; *this > t; return (unsigned int )t; }
	int parsing_names_query();
	int parsing_coords_query();
	int parsing_players_data_query();
	};


/*****************************************************************
				Prototypes
*****************************************************************/
namespace KDWIN {
int create_server(int port);
void destroy_server();
}

int connect_to_server(ServerFindChain* p);
int restore_connection();
void disconnect_from_server();

void set_time_by_server(int n_measures = 16);
int set_world(int world,int world_y_size);
void leave_world();
void total_players_data_list_query();

void send_player_body(PlayerBody& body);

void short_network_analysis(XBuffer& out);
void network_analysis(XBuffer& out,int integral);
void delay(int msec);

//zmod
int send_server_data(char* name, ServerData* data, zServerData* zdata); // returns 1 if _You_'ve configured server, otherwise data and name will be changed
int is_current_server_configured(char* name, ServerData* data, zServerData* zdata);  // changes name and data if server is configured
void void_network_quant(); // receives messages only

int NetInit(ServerFindChain* p);

struct NetRndType
{
	unsigned NetRndVal;
	void Init(unsigned d) { NetRndVal=d; }
	NetRndType() { NetRndVal=0; }
	inline unsigned Get(unsigned m){
		NetRndVal ^= NetRndVal >> 3;
		NetRndVal ^= NetRndVal << 28;
		NetRndVal &= 0x7FFFFFFF;
		if(!m) return 0;
		return NetRndVal%m;
	};	
};
#ifndef CLOCK
#define CLOCK() 	(clock()*18/1000)
#endif
#define GLOBAL_CLOCK()	(round(clock()*(256./1000)) - global_clock_tau)
#define LOCAL_CLOCK()	(round(clock()*(256./1000)))
#define age_of_current_game() ((((int)GLOBAL_CLOCK() - (int)game_birth_time_offset) / 256) >> 8)
//#define age_of_current_game() (((int)((clock()/1000.0) - global_clock_tau) - (int)game_birth_time_offset) >> 8)

#define START_TIMER(interval)	unsigned int _end_time_ = clock() + interval;
#define CHECK_TIMER()		((int)(clock() - _end_time_) < 0)
#define IS_FUTURE(time)		((int)((time) - clock()) > 0)
#define IS_PAST(time)		((int)(clock() - (time)) > 0)

/*****************************************************************
				Externs
*****************************************************************/
extern char* host_name;
extern int host_port;
extern char* proxy_name;
extern int proxy_port;
extern int non_blocking_mode;
extern InputEventBuffer events_in;
extern OutputEventBuffer events_out;
extern int global_clock_tau;
extern int time_interval_t0;
extern unsigned int game_birth_time_offset;
extern XBuffer network_analysis_buffer;
extern int average_lag;
extern XQueue<unsigned int> lag_averaging_t0;

extern int NetworkON;
extern NetRndType NetRnd;
extern int zGameBirthTime;
extern int CurrentWorld;
extern const char* CurPlayerName;

extern PlayerBody my_player_body;
extern PlayerData* my_player_data;
extern PlayersList players_list;
extern ServerData my_server_data;
extern ServersList avaible_servers;
extern MessageDispatcher message_dispatcher;

//zmod
extern zServerData z_my_server_data;
extern zCreateObjectQueue* z_create_object_queue;

/*****************************************************************
 Структура идентификатора объекта:
 [признак глобальности - 1][station - 5][world - 4][тип объекта - 6][counter - 16]
 первые восемь типов - личные вещи игрока
 [1 - 10] - автоматически удаляемые типы
station = 00000 - объект, не имеющий создателя
всего может быть 31 игрок, [1 - 31]
*****************************************************************/

extern int GlobalStationID; // PlayerID
extern int LocalStationID; //GlobalStationID << 26
extern int LocalStationCounter[MAX_NID_OBJECT];
extern int LocalNetEnvironment; //LocalStationID | CurrentWorld << 22)
extern int object_ID_offsets[16];
extern int stuff_ID_offsets;


#define CREATE_NET_ID(d) ((++LocalStationCounter[(d & (~(1 << 31))) >> 16] & 0xffff) | d | LocalNetEnvironment)
#define CREATE_STUFF_NET_ID(d) (((++LocalStationCounter[(d & (~(1 << 31))) >> 16] + stuff_ID_offsets) & 0xffff) | d | LocalNetEnvironment)
#define GET_NETWORK_ID(d) (d & ((63 << 16) | (1 << 31)))
#define GET_COUNTER_INDEX(d)	((d & (~(1 << 31))) >> 16)

const int NET_SBUNCH_COUNTER = 83;

void NetCheckRemovePlayer(PlayerData* p);
#endif
