//zmod
#include "zmod_client.h"

#ifdef _NetRally_
#include "NetRally.h"
#else
#include "global.h"
#include "lang.h"
#include "3d/3d_math.h"
#include "sound/hsound.h"
#endif

#ifndef _WIN32
#include <arpa/inet.h> // ntohl() FIXME: remove
#endif
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <vector>
#include "runtime.h"
#include "xgraph.h"
#include "network.h"

extern int MP_GAME;
extern XStream fout;
extern int frame;
extern int GlobalExit;

#define CLIENT_VERSION	4
#define SERVER_VERSION	4

//zmod
int zserver_version = 0;

int NetworkON = 0;
NetRndType NetRnd;

#define PLAYER_REMOVAL_EVENT(p) {NetCheckRemovePlayer(p);}

#ifdef _DEBUG
#define EVENTS_LOG
#define _FOUT_
#endif

#ifdef EVENTS_LOG
#define IN_EVENTS_LOG(id) { fout < "\t\t\t> ["  <= (GLOBAL_CLOCK() >> 8) < ":" <=  frame < "]" < #id < "\n"; }
#define IN_EVENTS_LOG1(id,code) { fout < "\t\t\t> ["  <= (GLOBAL_CLOCK() >> 8) < ":" <=  frame < "]" < #id < ": " <= code < "\n"; }
#define OUT_EVENTS_LOG(id) { fout < "< ["  <= (GLOBAL_CLOCK() >> 8) < ":" <=  frame < "]" < #id < "\n"; }
#define OUT_EVENTS_LOG1(id,code) { fout < "< ["  <= (GLOBAL_CLOCK() >> 8) < ":" <=  frame < "]" < #id < ": " <= code < "\n"; }
#else
#define IN_EVENTS_LOG(id)
#define IN_EVENTS_LOG1(id,code)
#define OUT_EVENTS_LOG(id)
#define OUT_EVENTS_LOG1(id,code)
#endif

#define DOUT(str) { std::cout<<str<<"\n"; }
#define DOUT1(str, code) { std::cout<<str<<", code: "<<code<<"\n"; }

const int IN_BUFFER_SIZE = 128000;
const int OUT_BUFFER_SIZE = 256000;

#ifdef _ROAD_
extern int iProxyUsage;
extern char* iProxyServer;
extern int iProxyPort;
#else
int iProxyUsage = 1;
#define iProxyServer proxy_name
#define  iProxyPort proxy_port
int proxy_port = 1080;
char* proxy_name = 0;
#endif

int non_blocking_mode = 1;
int enable_transferring = 0;
int enable_send = 1;
int average_lag = 0;
XQueue<unsigned int> lag_averaging_t0(32);

int global_clock_tau = 0;
unsigned int game_birth_time_offset;
double response_time = 0;
double time_synchronization_sigma;

int n_sended_events, n_sended_bytes;
int n_received_events, n_received_bytes;
int network_clock, network_frame;
int n_sended_events_global, n_sended_bytes_global;
int n_received_events_global, n_received_bytes_global;
int network_clock_global = -1, network_frame_global;
int delay_time,delay_time_global;
int delay_time_counter,delay_time_counter_global;
int now_unsent_size,global_unsent_size = 0;
int set_world_status = 0;


XBuffer network_analysis_buffer(10000);
InputEventBuffer events_in(IN_BUFFER_SIZE);
OutputEventBuffer events_out(OUT_BUFFER_SIZE);

ServerData my_server_data;
ServersList avaible_servers;

//zmod
zServerData z_my_server_data;
zCreateObjectQueue* z_create_object_queue;

PlayerBody my_player_body;
PlayerData* my_player_data = 0;
PlayersList players_list;

static unsigned int total_players_data_list_query_last_time = 0;
static int total_players_data_list_query_pending = 0;
static const unsigned int TOTAL_PLAYERS_DATA_LIST_QUERY_RETRY_MS = 1000;

MessageDispatcher message_dispatcher;

XSocket main_socket;
ServerFindChain current_server_addr;
int number_of_reconnection_attempt = 5;

int object_ID_offsets[16];

#ifndef GIT_COMMIT_HASH
#define GIT_COMMIT_HASH "unknown"
#endif
#ifndef GITHUB_COMMIT_HASH
#define GITHUB_COMMIT_HASH ""
#endif
#ifndef GIT_BRANCH
#define GIT_BRANCH "unknown"
#endif
#ifndef GITHUB_BRANCH
#define GITHUB_BRANCH ""
#endif

static FILE* network_log_file = NULL;
static unsigned int network_log_last_flush = 0;
static unsigned int network_log_skipped_update_count = 0;
static unsigned int network_log_skipped_update_last_ms = 0;
static int network_log_last_world = -1000000;

static const char* network_event_name(int event_ID)
{
	int event = (event_ID & AUXILIARY_EVENT) ? event_ID : (event_ID & (~ECHO_EVENT));
	switch(event){
		case CREATE_OBJECT: return "CREATE_OBJECT";
		case DELETE_OBJECT: return "DELETE_OBJECT";
		case UPDATE_OBJECT: return "UPDATE_OBJECT";
		case HIDE_OBJECT: return "HIDE_OBJECT";
		case GAMES_LIST_QUERY: return "GAMES_LIST_QUERY";
		case TOP_LIST_QUERY: return "TOP_LIST_QUERY";
		case ATTACH_TO_GAME: return "ATTACH_TO_GAME";
		case RESTORE_CONNECTION: return "RESTORE_CONNECTION";
		case CLOSE_SOCKET: return "CLOSE_SOCKET";
		case REGISTER_NAME: return "REGISTER_NAME";
		case SERVER_TIME_QUERY: return "SERVER_TIME_QUERY";
		case SET_WORLD: return "SET_WORLD";
		case LEAVE_WORLD: return "LEAVE_WORLD";
		case SET_POSITION: return "SET_POSITION";
		case TOTAL_PLAYERS_DATA_QUERY: return "TOTAL_PLAYERS_DATA_QUERY";
		case SET_GAME_DATA: return "SET_GAME_DATA";
		case GET_GAME_DATA: return "GET_GAME_DATA";
		case SET_PLAYER_DATA: return "SET_PLAYER_DATA";
		case DIRECT_SENDING: return "DIRECT_SENDING";
		case GAMES_LIST_RESPONSE: return "GAMES_LIST_RESPONSE";
		case TOP_LIST_RESPONSE: return "TOP_LIST_RESPONSE";
		case TOTAL_LIST_OF_PLAYERS_DATA: return "TOTAL_LIST_OF_PLAYERS_DATA";
		case ATTACH_TO_GAME_RESPONSE: return "ATTACH_TO_GAME_RESPONSE";
		case RESTORE_CONNECTION_RESPONSE: return "RESTORE_CONNECTION_RESPONSE";
		case SERVER_TIME: return "SERVER_TIME";
		case SERVER_TIME_RESPONSE: return "SERVER_TIME_RESPONSE";
		case SET_WORLD_RESPONSE: return "SET_WORLD_RESPONSE";
		case GAME_DATA_RESPONSE: return "GAME_DATA_RESPONSE";
		case DIRECT_RECEIVING: return "DIRECT_RECEIVING";
		case PLAYERS_NAME: return "PLAYERS_NAME";
		case PLAYERS_POSITION: return "PLAYERS_POSITION";
		case PLAYERS_WORLD: return "PLAYERS_WORLD";
		case PLAYERS_STATUS: return "PLAYERS_STATUS";
		case PLAYERS_DATA: return "PLAYERS_DATA";
		case PLAYERS_RATING: return "PLAYERS_RATING";
		case zSERVER_VERSION_RESPONSE: return "zSERVER_VERSION_RESPONSE";
		case zGAME_DATA_RESPONSE: return "zGAME_DATA_RESPONSE";
		case zTIME_RESPONSE: return "zTIME_RESPONSE";
		case zCREATE_OBJECT_BY_CLIENT: return "zCREATE_OBJECT_BY_CLIENT";
		case zCREATE_OBJECT_BY_SERVER: return "zCREATE_OBJECT_BY_SERVER";
		default: return "UNKNOWN_EVENT";
	}
}

static const char* network_object_type_name(int object_ID)
{
	switch(GET_NETWORK_ID(object_ID)){
		case NID_GLOBAL: return "GLOBAL";
		case NID_DEVICE: return "DEVICE";
		case NID_SLOT: return "SLOT";
		case NID_SHELL: return "SHELL";
		case NID_VANGER: return "VANGER";
		case NID_STUFF: return "STUFF";
		case NID_SENSOR: return "SENSOR";
		case NID_TNT: return "TNT";
		case NID_TERRAIN: return "TERRAIN";
		default: return "UNKNOWN";
	}
}

static int network_log_should_log_object_event(int event_ID,int object_ID)
{
	int event = event_ID & (~ECHO_EVENT);
	if(event != UPDATE_OBJECT)
		return 1;

	switch(GET_NETWORK_ID(object_ID)){
		case NID_GLOBAL:
		case NID_DEVICE:
		case NID_SLOT:
		case NID_VANGER:
		case NID_STUFF:
			return 1;
		default:
			return 0;
	}
}

static void network_log_timestamp(char* buf,size_t size)
{
	using namespace std::chrono;
	system_clock::time_point now = system_clock::now();
	time_t tt = system_clock::to_time_t(now);
	int ms = (int)(duration_cast<milliseconds>(now.time_since_epoch()).count() % 1000);
	struct tm tmv;
#ifdef _WIN32
	gmtime_s(&tmv,&tt);
#else
	gmtime_r(&tt,&tmv);
#endif
	snprintf(buf,size,"%04d-%02d-%02dT%02d:%02d:%02d.%03dZ",
		tmv.tm_year + 1900,tmv.tm_mon + 1,tmv.tm_mday,
		tmv.tm_hour,tmv.tm_min,tmv.tm_sec,ms);
}

static void network_log_server_address(const ServerFindChain* p,char* buf,size_t size)
{
	if(!p){
		snprintf(buf,size,"<unknown>");
		return;
	}
	if(p->domain_name)
		snprintf(buf,size,"%s:%d",p->domain_name,p->port);
	else
		snprintf(buf,size,"%d.%d.%d.%d:%d",
			p->IP & 0xff,(p->IP >> 8) & 0xff,(p->IP >> 16) & 0xff,(p->IP >> 24) & 0xff,p->port);
}

static const char* network_build_hash()
{
	return GITHUB_COMMIT_HASH[0] ? GITHUB_COMMIT_HASH : GIT_COMMIT_HASH;
}

static const char* network_build_branch()
{
	return GITHUB_BRANCH[0] ? GITHUB_BRANCH : GIT_BRANCH;
}

static void network_log_vprintf(const char* tag,const char* fmt,va_list args)
{
	if(!network_log_file)
		return;

	char ts[64];
	network_log_timestamp(ts,sizeof(ts));
	if(network_log_last_world != CurrentWorld){
		fprintf(network_log_file,"[%s tick=%d frame=%d world=%d station=%d] CONTEXT CurrentWorld changed old=%d new=%d\n",
			ts,(int)GLOBAL_CLOCK(),frame,CurrentWorld,GlobalStationID,network_log_last_world,CurrentWorld);
		network_log_last_world = CurrentWorld;
	}
	fprintf(network_log_file,"[%s tick=%d frame=%d world=%d station=%d] %s ",
		ts,(int)GLOBAL_CLOCK(),frame,CurrentWorld,GlobalStationID,tag ? tag : "NET");
	vfprintf(network_log_file,fmt,args);
	fputc('\n',network_log_file);

	unsigned int now = SDL_GetTicks();
	if(now - network_log_last_flush > 1000){
		fflush(network_log_file);
		network_log_last_flush = now;
	}
}

void network_log_printf(const char* tag,const char* fmt,...)
{
	va_list args;
	va_start(args,fmt);
	network_log_vprintf(tag,fmt,args);
	va_end(args);
}

static void network_log_open(const ServerFindChain* p,const char* reason)
{
	if(network_log_file){
		fflush(network_log_file);
		fclose(network_log_file);
		network_log_file = NULL;
	}

	network_log_file = fopen("network-client.log","w");
	network_log_last_flush = SDL_GetTicks();
	network_log_skipped_update_count = 0;
	network_log_skipped_update_last_ms = network_log_last_flush;
	network_log_last_world = CurrentWorld;
	if(!network_log_file)
		return;

	char server[256];
	network_log_server_address(p,server,sizeof(server));
	network_log_printf("client_start",
		"reason=%s build_hash=%s branch=%s protocol_version=%d/%d zserver_version=%d fps_mode=%.0ffps fps_coeff=%.3f resolution=%dx%d server_address=%s game_id=%d player_name=\"%s\" GlobalStationID=%d CurrentWorld=%d",
		reason ? reason : "connect",
		network_build_hash(),
		network_build_branch(),
		CLIENT_VERSION,
		SERVER_VERSION,
		zserver_version,
		GAME_TIME_COEFF > 1.5 ? 60.0 : 20.0,
		GAME_TIME_COEFF,
		XGR_MAXX,
		XGR_MAXY,
		server,
		p ? p->game_ID : current_server_addr.game_ID,
		CurPlayerName ? CurPlayerName : "",
		GlobalStationID,
		CurrentWorld);
}

static void network_log_close(const char* reason)
{
	if(!network_log_file)
		return;
	network_log_printf("client_stop","reason=%s",reason ? reason : "close");
	fflush(network_log_file);
	fclose(network_log_file);
	network_log_file = NULL;
}

void network_log_object_event(const char* direction,int event_ID,int object_ID,int creator,int time,int x,int y,int radius,int body_size,const char* decision)
{
	int event = event_ID & (~ECHO_EVENT);
	if(!network_log_should_log_object_event(event,object_ID)){
		network_log_skipped_update_count++;
		unsigned int now = SDL_GetTicks();
		if(now - network_log_skipped_update_last_ms > 5000){
			network_log_printf(direction ? direction : "OBJ",
				"%s summary skipped_ordinary_updates=%u",
				network_event_name(event),
				network_log_skipped_update_count);
			network_log_skipped_update_count = 0;
			network_log_skipped_update_last_ms = now;
		}
		return;
	}

	network_log_printf(direction ? direction : "OBJ",
		"%s id=0x%08X id_dec=%d station=%d world=%d type=%s type_id=%d counter=%d is_global=%d is_private=%d is_players_object=%d is_static=%d creator=%d time=%d x=%d y=%d radius=%d body_size=%d current_world=%d decision=%s",
		network_event_name(event),
		(unsigned int)object_ID,
		object_ID,
		GET_STATION(object_ID),
		GET_WORLD(object_ID),
		network_object_type_name(object_ID),
		(object_ID >> 16) & 63,
		object_ID & 0xffff,
		!NON_GLOBAL_OBJECT(object_ID),
		!!PRIVATE_OBJECT(object_ID),
		!!PLAYERS_OBJECT(object_ID),
		!NON_STATIC(object_ID),
		creator,
		time,
		x,
		y,
		radius,
		body_size,
		CurrentWorld,
		decision ? decision : "parsed");
}

void network_log_item_event(const char* tag,int NetID,int NetDeviceID,int NetOwner,int OwnerNetID,int DataID,int actint_type,int actint_data0,int actint_data1,int CreateMode,int OutFlag,int Status,int Visibility,const char* extra)
{
	network_log_printf(tag ? tag : "ITEM",
		"NetID=0x%08X NetID_dec=%d NetDeviceID=0x%08X NetDeviceID_dec=%d NetOwner=0x%08X NetOwner_dec=%d OwnerNetID=0x%08X OwnerNetID_dec=%d DataID=%d ActIntBuffer.type=%d ActIntBuffer.data0=%d ActIntBuffer.data1=%d CreateMode=%d OutFlag=%d Status=0x%X Visibility=%d current_world=%d %s",
		(unsigned int)NetID,
		NetID,
		(unsigned int)NetDeviceID,
		NetDeviceID,
		(unsigned int)NetOwner,
		NetOwner,
		(unsigned int)OwnerNetID,
		OwnerNetID,
		DataID,
		actint_type,
		actint_data0,
		actint_data1,
		CreateMode,
		OutFlag,
		Status,
		Visibility,
		CurrentWorld,
		extra ? extra : "");
}



extern	void* XGR_hWnd;
namespace KDWIN {

//PROCESS_INFORMATION ProcessInformation;
//STARTUPINFO StartUpInfo;

int create_server(int port)
{
	if(!XSocketLocalHostADDR.host)
		ErrH.Abort("You have an incompatible network configuration. Please consult Readme.txt on how to configure your network for Vangers multiplayer.");
	avaible_servers.clear_states();
	if(avaible_servers.talk_to_server(ntohl(XSocketLocalHostADDR.host),port,0,1))
		return avaible_servers.size();

	destroy_server(); // if  You've already created server on another port

	XBuffer cmd_line;
	cmd_line < "MP_SERVER\\VangersServer.exe /t900 /i /p" <= port;

/*	ZeroMemory(&StartUpInfo,sizeof(STARTUPINFO));
	StartUpInfo.cb = sizeof(STARTUPINFO);
	StartUpInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartUpInfo.wShowWindow = SW_HIDE;
	 if(CreateProcess(
					0,	// pointer to name of executable module
					cmd_line.GetBuf(),	// pointer to command line string
					0,	// pointer to process security attributes
					0,	// pointer to thread security attributes
					0,	// handle inheritance flag
//zmod
//                  NORMAL_PRIORITY_CLASS,  // creation flags //zMod
                    HIGH_PRIORITY_CLASS,  // creation flags //zMod
					0,	// pointer to new environment block
					0,	// pointer to current directory name
					&StartUpInfo,	// pointer to STARTUPINFO
					&ProcessInformation	// pointer to PROCESS_INFORMATION
					)){
		CloseHandle(ProcessInformation.hThread);

		if(avaible_servers.talk_to_server(XSocketLocalHostADDR,port,0,1))
			return avaible_servers.size();
		}
*/
	return 0;
}
void destroy_server()
{
/*	HANDLE hProcess = ProcessInformation.hProcess;
	if(hProcess){
//		TerminateProcess(ProcessInformation.hProcess,0);
		CloseHandle(ProcessInformation.hProcess);
		}
*/}
}
/*****************************************************************
				ServerFindChain
*****************************************************************/
ServerFindChain::ServerFindChain(int IP,int port,char* domain_name,int game_ID,char* game_name)
{
	ServerFindChain::domain_name = domain_name;
	ServerFindChain::IP = IP;
	ServerFindChain::port = port;
	ServerFindChain::game_ID = game_ID;
	configured = 0;
	prev = next = 0;
	list = 0;
	XBuffer str_buf;
	if(!game_ID) {
	    if (lang() == RUSSIAN) {
			//CP866 Новая игра на
			const unsigned char new_game_on[] = {0x8D, 0xAE, 0xA2, 0xA0, 0xEF, 0x20, 0xA8, 0xA3, 0xE0, 0xA0, 0x20, 0xAD, 0xA0, 0x20, 0x00};
			str_buf < (const char *)new_game_on;
        } else {
            str_buf < "New Game on ";
        }
	}
	if(!game_name)
		if(domain_name)
			str_buf < domain_name;
		else
			str_buf <= (IP & 0xff) < "." <= ((IP >> 8) & 0xff) < "." <= ((IP >> 16) & 0xff) < "." <= ((IP >> 24) & 0xff);
	else
		str_buf < game_name;
	strncpy(name, str_buf.GetBuf(), 50);
	name[50] = 0;
}
ServerFindChain::~ServerFindChain()
{}

// socks proxies handled by tsocks/FreeCap
void ServerFindChain::connect(XSocket &target)
{
	XSocket socket;
	if(IP)
		socket.open(IP,port);
	else
// 		if(!iProxyUsage || !iProxyServer)
			socket.open(domain_name,port);
// 		else
// 			socket.open_by_socks5(domain_name,port,iProxyServer,iProxyPort);
	target=socket;
}

/*****************************************************************
				Servers List
*****************************************************************/
int identification(XSocket& socket)
{
	char string[256] = "";
	memset(string,0,256);
	unsigned int len,identificated = 0;
	int server_protocol = -1;
	int server_replied = 0;
	int unexpected_response = 0;
	START_TIMER(60*1000);
	const char* request_str = "Vivat Sicher, Rock'n'Roll forever!!!";
	const char* response_str = "Enter, my son, please...";
	strcpy(string, request_str);
	string[strlen(string) + 1] = CLIENT_VERSION;
	socket.send(string,strlen(string) + 2);
	while(CHECK_TIMER()){
		memset(string,0,256);
		if((len = socket.receive(string, 255, 1000)) != 0){
			server_replied = 1;
			if(strcmp(string,response_str)){
				unexpected_response = 1;
				break;
			}
			if(len > strlen(response_str) + 1)
				server_protocol = (unsigned char)string[strlen(response_str) + 1];
			if(!SERVER_VERSION || server_protocol == SERVER_VERSION)
				identificated = 1;
			break;
		}
	}
	if(!identificated) {
		char error_message[512];
		if(server_replied && !unexpected_response && server_protocol >= 0) {
			snprintf(
				error_message,
				sizeof(error_message),
				"Версия сетевого протокола не совпадает. Клиент ожидает protocol %d, сервер ответил protocol %d. Обновите клиент и сервер.",
				SERVER_VERSION,
				server_protocol
			);
		} else if(server_replied) {
			snprintf(
				error_message,
				sizeof(error_message),
				"Сервер отклонил сетевое подключение до handshake. Проверьте, что клиент и сервер обновлены до protocol %d.",
				SERVER_VERSION
			);
		} else {
			snprintf(
				error_message,
				sizeof(error_message),
				"Сервер не отвечает во время сетевого handshake. Проверьте адрес сервера и версию protocol %d.",
				SERVER_VERSION
			);
		}
		std::cout<<"Network:identificated is wrong! SV:"<<SERVER_VERSION<<" SV2:"<<server_protocol<<" response:"<<string<<std::endl;
		socket.close();
		ErrH.Abort(error_message, XERR_USER);
		return 0;
	}
// zMod fixed ---------------------------------------------------------

    XBuffer zbuffer(10000);
    zbuffer < short(1) < char(SERVER_TIME_QUERY);
    if(!socket.send(zbuffer.GetBuf(),zbuffer.tell()))
        return 0;

    unsigned int z_end_time_ = SDL_GetTicks() + 2*60*1000;
    while (((int)(SDL_GetTicks() - z_end_time_) < 0))
	if (socket.receive(zbuffer.GetBuf(), zbuffer.length(), 1000))
	   break;

    unsigned char zevent_ID;
    short zevent_size;
    int zresponse;

    zbuffer.set(0);
    zbuffer > zevent_size > zevent_ID > zresponse;

	if(zevent_ID == SERVER_TIME)
		zserver_version = 1;

    //std::cout<<"Client auth: "<<(int)zevent_ID<<" "<<zSERVER_VERSION_RESPONSE<<" "<<(int)zresponse<<" "<<zCLIENT_VERSION<<std::endl;
	if (zevent_ID == zSERVER_VERSION_RESPONSE) {
		if (zCLIENT_VERSION == zresponse) {
			zserver_version = zresponse;
		} else {
		//NEED SEE!
		//    ErrH.Abort(zSTR_WRONG_VERSION);
		 //   return 0;
		}
	}
// /zMod ---------------------------------------------------------

	return 1;
}
void ServersList::clear_states()
{
	ServerFindChain* p;
	while((p = first()) != 0){
		remove(p);
		delete p;
		}
}
int ServersList::talk_to_server(int IP,int port,char* domain_name,int only_new_game)
{
	XSocket sock;
	if(IP) {
		sock.open(IP,port);
	} else {
		if(!domain_name)
			return 0;
// 		if(!iProxyUsage || !iProxyServer)
			sock.open(domain_name,port);
// 		else
// 			sock.open_by_socks5(domain_name,port,iProxyServer,iProxyPort);
	}
	if(!sock || !identification(sock))
		return 0;

	XBuffer servers_buffer(10000);
	servers_buffer < short(1) < char(GAMES_LIST_QUERY);
	if(!sock.send(servers_buffer.GetBuf(),servers_buffer.tell()))
		return 0;

	START_TIMER(60*1000);
	while(CHECK_TIMER())
		if(sock.receive(servers_buffer.GetBuf(), servers_buffer.length(), 1000))
			break;

	ServerFindChain* p;
	unsigned char n_games,event_ID;
	int game_ID;
	char game_name[257];
	short event_size;
	servers_buffer.set(0);
	servers_buffer > event_size > event_ID > n_games;
	if(event_ID != GAMES_LIST_RESPONSE)
		return 0;
	if(only_new_game)
		n_games = 0;
	for(int i = 0;i < n_games;i++) {
		servers_buffer > game_ID > game_name;
		std::cout<<"game_ID:"<<game_ID<<" game_name:"<<game_name<<std::endl;
		p = new ServerFindChain(IP, port, domain_name, game_ID, game_name);
		append(p);
	}
	p = new ServerFindChain(IP,port,domain_name,0,0);
	append(p);
	return n_games + 1;
}

int ServersList::find_servers_in_the_internet(char* host_name,int host_port)
{
	clear_states();
	strcpy(domain_name, host_name);
	return talk_to_server(0, host_port, domain_name);
}

/***********************************************************************
				Output Event Buffer
***********************************************************************/
OutputEventBuffer::OutputEventBuffer(unsigned int size)
: XBuffer(size)
{
	clear();
}
void OutputEventBuffer::clear()
{
	n_event = 0;
	offset = 0;
	pointer_to_the_first_event = 0;
	pointer_to_the_last_event = 0;
	pointer_to_size_of_event = -1;
}
int OutputEventBuffer::full_event_size(int index)
{
	return *((short*)(address() + index)) + 2;
}
void OutputEventBuffer::create_permanent_object(int ID,int x,int y,int radius,int flags)
{
	n_event++;
	pointer_to_size_of_event = tell();
	*this < short(0) < (unsigned char)(CREATE_PERMANENT_OBJECT | flags);
	*this < ID < (unsigned int)GLOBAL_CLOCK() < (unsigned short)x < (unsigned short)y < (unsigned short)radius;
	OUT_EVENTS_LOG1(CREATE_PERMANENT_OBJECT,ID);
}
void OutputEventBuffer::update_object(int ID,int x,int y,int flags)
{
	n_event++;
	pointer_to_size_of_event = tell();
	*this < short(0) < (unsigned char)(UPDATE_OBJECT | flags);
	*this < ID < (unsigned int)GLOBAL_CLOCK() < (unsigned short)x < (unsigned short)y;
	OUT_EVENTS_LOG1(UPDATE_OBJECT,ID);
}
void OutputEventBuffer::delete_object(int ID)
{
	n_event++;
	pointer_to_size_of_event = tell();
	*this < short(0) < (unsigned char)DELETE_OBJECT < ID < (unsigned int)GLOBAL_CLOCK();
	OUT_EVENTS_LOG1(DELETE_OBJECT,ID);
}
void OutputEventBuffer::begin_create_z_object(int Type, int ID)
{
	n_event++;
	pointer_to_size_of_event = tell();
	*this < short(0) < (unsigned char)zCREATE_OBJECT_BY_CLIENT < (unsigned int)GLOBAL_CLOCK() < (unsigned char)Type < (unsigned char)ID;
	OUT_EVENTS_LOG(zCREATE_OBJECT_BY_CLIENT);
}
void OutputEventBuffer::begin_server_data()
{
	n_event++;
	pointer_to_size_of_event = tell();
	*this < short(0) < (unsigned char)SET_GAME_DATA;
	OUT_EVENTS_LOG(SET_GAME_DATA);
}
void OutputEventBuffer::begin_direct_send(unsigned int mask)
{
	n_event++;
	pointer_to_size_of_event = tell();
	*this < short(0) < (unsigned char)DIRECT_SENDING < mask;
	OUT_EVENTS_LOG(DIRECT_SENDING);
}
void OutputEventBuffer::set_world(int world,int y_size_of_world)
{
	n_event++;
	*this < short(4) < (unsigned char)SET_WORLD < (unsigned char)world < (unsigned short)y_size_of_world;
	OUT_EVENTS_LOG1(SET_WORLD,world);
	network_log_printf("OUT","SET_WORLD requested_world=%d world_y_size=%d",world,y_size_of_world);
}
void OutputEventBuffer::set_position(int x,int y,int y_half_size_of_screen)
{
	n_event++;
	*this < short(7) < (unsigned char)SET_POSITION < (unsigned short)x < (unsigned short)y < (unsigned short)y_half_size_of_screen;
	OUT_EVENTS_LOG1(SET_POSITION,y);
	network_log_printf("OUT","SET_POSITION x=%d y=%d y_half_size_of_screen=%d",x,y,y_half_size_of_screen);
	for(int i = pointer_to_the_first_event;i < pointer_to_the_last_event;i += full_event_size(i)){
		if(*((unsigned char*)(address() + i + 2)) == SET_POSITION){
			int event_size = full_event_size(i);
			memmove(address() + i, address() + i + event_size, tell() - (i + event_size));
			offset -= event_size;
			pointer_to_the_last_event -= event_size;
			n_event--;
			OUT_EVENTS_LOG(Pack_SetPosition);
			break;
			}
		}
}

void OutputEventBuffer::register_name(char* name, char* password)
{
	n_event++;
	*this < short(1 + strlen(name) + 1 + strlen(password) + 1) < (unsigned char)(REGISTER_NAME) < name < char(0) < password < char(0);
	OUT_EVENTS_LOG(REGISTER_NAME);
	network_log_printf("OUT","REGISTER_NAME player_name=\"%s\"",name ? name : "");
}
void OutputEventBuffer::begin_event(int event_ID)
{
	if(pointer_to_size_of_event != -1)
		ErrH.Abort("There wasn't an end of event");
	pointer_to_size_of_event = offset;
	*this < short(0) < (unsigned char)event_ID;
}

static void network_log_out_event_from_buffer(char* event_ptr)
{
	short event_size = *((short*)event_ptr);
	unsigned char event_ID = *((unsigned char*)(event_ptr + 2));
	int event = event_ID & (~ECHO_EVENT);

	switch(event){
		case CREATE_OBJECT:{
			int object_ID = *((int*)(event_ptr + 3));
			unsigned int time = *((unsigned int*)(event_ptr + 7));
			unsigned short x = *((unsigned short*)(event_ptr + 11));
			unsigned short y = *((unsigned short*)(event_ptr + 13));
			unsigned short radius = *((unsigned short*)(event_ptr + 15));
			network_log_object_event("OUT",event,object_ID,GlobalStationID,(int)time,(int)x,(int)y,(int)radius,event_size - 15,"queued");
			break;
		}
		case UPDATE_OBJECT:{
			int object_ID = *((int*)(event_ptr + 3));
			unsigned int time = *((unsigned int*)(event_ptr + 7));
			unsigned short x = *((unsigned short*)(event_ptr + 11));
			unsigned short y = *((unsigned short*)(event_ptr + 13));
			network_log_object_event("OUT",event,object_ID,GlobalStationID,(int)time,(int)x,(int)y,-1,event_size - 13,"queued");
			break;
		}
		case DELETE_OBJECT:{
			int object_ID = *((int*)(event_ptr + 3));
			unsigned int time = *((unsigned int*)(event_ptr + 7));
			network_log_object_event("OUT",event,object_ID,GlobalStationID,(int)time,-1,-1,-1,event_size - 9,"queued");
			break;
		}
		default:
			network_log_printf("OUT","%s body_size=%d raw_event=0x%02X",network_event_name(event),event_size - 1,event_ID);
			break;
	}
}

void OutputEventBuffer::end_body()
{
	int i;
	if(pointer_to_size_of_event < 0)
		ErrH.Abort("There wasn't a beginning of event");

	//std::cout<<"OutputEventBuffer::end_body size:"<<tell() - pointer_to_size_of_event - sizeof(short int)<<std::endl;
	*(short*)(address() + pointer_to_size_of_event) = tell() - pointer_to_size_of_event - sizeof(short int);
	network_log_out_event_from_buffer(address() + pointer_to_size_of_event);

	int ev_ID,event_ID = *(unsigned char*)(address() + pointer_to_size_of_event + 2);
	if((event_ID & (~ECHO_EVENT)) == UPDATE_OBJECT) {
		int object_ID = *(int*)(address() + pointer_to_size_of_event + 3);
		for(i = pointer_to_the_first_event; i < pointer_to_the_last_event; i += full_event_size(i)) {
			ev_ID = *((unsigned char*)(address() + i + 2));
			if((ev_ID & (~ECHO_EVENT)) == UPDATE_OBJECT) {
				if(*((int*)(address() + i + 3)) == object_ID) {
					int event_size = full_event_size(i);
					memmove(address() + i, address() + i + event_size, tell() - (i + event_size));
					*(unsigned char*)(address() + pointer_to_size_of_event + 2) |= ev_ID & ECHO_EVENT;
					offset -= event_size;
					pointer_to_the_last_event -= event_size;
					n_event--;
					OUT_EVENTS_LOG1(Pack_update,object_ID);
					break;
				}
			}
		}
	}
	pointer_to_size_of_event = -1;
}
int OutputEventBuffer::send(int system_send, XSocket& sock)
{
	if(pointer_to_size_of_event > 0)
		ErrH.Abort("There wasn't an end of event");
	int sent_size = 0;
	//if(sock() && (system_send | enable_send))
	if(sock()){
		sent_size = sock.send(address(),tell());
		//if(!system_send)
		//	enable_send = 0;
		}
	int unsent_size = tell() - sent_size;
	n_sended_bytes += sent_size;
	if(sent_size == 0 && tell() > 0)
		network_log_printf("SOCKET_ERROR","send_failed requested_size=%d sent_size=%d socket_alive=%d",tell(),sent_size,sock());
#ifdef _FOUT_
	if(sent_size)
		fout < "Send: " <= SDL_GetTicks() < "\t" <= sent_size < "\t" <= unsent_size < "\t" <= n_event < "\n";
#endif
	if(!unsent_size){
		n_sended_events += n_event;
		pointer_to_the_first_event = 0;
		pointer_to_the_last_event = 0;
		n_event = 0;
		offset = 0;
		}
	else{
		OUT_EVENTS_LOG1(Send_would_block,unsent_size);
		network_log_printf("SOCKET_ERROR","send_would_block sent_size=%d unsent_size=%d n_event=%d",sent_size,unsent_size,n_event);
		now_unsent_size = unsent_size;

		while(pointer_to_the_first_event < sent_size){
			pointer_to_the_first_event += full_event_size(pointer_to_the_first_event);
			n_sended_events++;
			n_event--;
			}

		if(sent_size){
			memmove(buf,buf + sent_size,unsent_size);
			offset -= sent_size;
			pointer_to_the_first_event -= sent_size;
			}
		pointer_to_the_last_event = offset;
		}
	return sent_size;
}
int OutputEventBuffer::send_simple_query(int event,XSocket& sock)
{
	if(pointer_to_size_of_event > 0)
		ErrH.Abort("There wasn't an end of event");
	n_event++;
	*this < short(1) < (unsigned char)event;
	OUT_EVENTS_LOG1(send_simple_query,event);
	network_log_printf("OUT","%s body_size=0",network_event_name(event));
	return send(1,sock);
}
int OutputEventBuffer::send_simple_query(int event, unsigned char data,XSocket& sock)
{
	n_event++;
	*this < short(2) < (unsigned char)event < data;
	OUT_EVENTS_LOG1(send_simple_query,event);
	network_log_printf("OUT","%s data0=%d body_size=1",network_event_name(event),(int)data);
	return send(1,sock);
}
int OutputEventBuffer::send_simple_query(int event, unsigned char data1, unsigned char data2,XSocket& sock)
{
	n_event++;
	*this < short(3) < (unsigned char)event < data1 < data2;
	OUT_EVENTS_LOG1(send_simple_query,event);
	network_log_printf("OUT","%s data0=%d data1=%d body_size=2",network_event_name(event),(int)data1,(int)data2);
	return send(1,sock);
}
void OutputEventBuffer::set_player_body(PlayerBody& body)
{
	n_event++;
	*this < short(1 + sizeof(PlayerBody)) < (unsigned char)SET_PLAYER_DATA;
	write(&body,sizeof(PlayerBody));
	OUT_EVENTS_LOG(SET_PLAYER_DATA);
	network_log_printf("OUT","SET_PLAYER_DATA body_size=%d NetID=0x%08X world=%d color=%d", (int)sizeof(PlayerBody), (unsigned int)body.NetID, body.world, body.color);
}

/***********************************************************************
				Input Event Buffer
***********************************************************************/
InputEventBuffer::InputEventBuffer(unsigned int size)
: XBuffer(size)
{
	reset();
}

void InputEventBuffer::reset()
{
	next_event_pointer = 0;
	filled_size = 0;
	offset = 0;
}

int InputEventBuffer::receive(XSocket& sock,int dont_free) {
	restore_connection();

	if(next_event_pointer != tell()) {
		XBuffer str;
		str < "Connection's problems: " <= event_ID < "  "  <= object_ID < "  "  <= next_event_pointer - tell();
		ErrH.Abort(str.GetBuf(), XERR_USER);
	}

	if(next_event_pointer && !dont_free) {
		if(filled_size != next_event_pointer) {
			memmove(address(), address() + next_event_pointer, filled_size - next_event_pointer);
		}
		filled_size -= next_event_pointer;
		offset = next_event_pointer = 0;
	}

	int add_size = sock.receive(address() + filled_size, length() - filled_size);
	filled_size += add_size;
	n_received_bytes += add_size;
	if(add_size)
		enable_send = 1;
	if(!add_size && !sock){
		network_log_printf("SOCKET_ERROR","receive_failed filled_size=%u next_event_pointer=%u",filled_size,next_event_pointer);
		network_log_printf("SOCKET_CLOSED","reason=receive_failed filled_size=%u next_event_pointer=%u",filled_size,next_event_pointer);
	}
#ifdef _FOUT_
	if(add_size)
		fout < "Receive: " <= SDL_GetTicks() < "\t" <= add_size < "\n";
#endif
	//std::cout<<"InputEventBuffer::receive "<<add_size<<" "<<std::endl;
	return next_event();
}

int InputEventBuffer::receive_waiting_for_event(int event, XSocket& sock,int skip_if_aint)
{
	//std::cout<<"InputEventBuffer::receive_waiting_for_event "<<event<<std::endl;
	receive(sock);
	if(!sock) {
		network_log_printf("WAIT","receive_waiting_for_event event=%s result=socket_closed",network_event_name(event));
		if(!skip_if_aint)
			{
			if (lang() == RUSSIAN) {
				ErrH.Abort("Сервер не отвечает", XERR_USER, event);
			} else {
				ErrH.Abort("Time out of Server's response receiving", XERR_USER, event);
			}
			}
		event_ID = 0;
		offset = next_event_pointer = 0;
		return 0;
	}
	START_TIMER(120*1000);
	for(;;) {
		while(current_event()) {
			//std::cout<<"current_event:"<<(int)current_event()<<" clock:"<<SDL_GetTicks()<<" _end_time_:"<<_end_time_<<std::endl;
			if(current_event() == event) {
				//std::cout<<"ok"<<std::endl;
				unsigned int size = (unsigned int)event_size + 2;
				if(next_event_pointer < size || next_event_pointer > filled_size) {
					ErrH.Abort("Bad network event buffer", XERR_USER, event);
					event_ID = 0;
					offset = next_event_pointer = 0;
					return 0;
				}
				unsigned int prefix = next_event_pointer - size;
				if(prefix > 0) {
					std::vector<char> event_copy(size);
					memcpy(event_copy.data(), buf + prefix, size);
					memmove(buf + size, buf, prefix);
					memcpy(buf, event_copy.data(), size);
					//offset = next_event_pointer = size - body_size;
					offset = next_event_pointer = 0;
					return next_event();
				}
				return event;
			}
			ignore_event();
			next_event();
		}

		if(!CHECK_TIMER())
			break;

		receive(sock,1);
		if(!sock)
			break;
	}
	if(!skip_if_aint)
        {
	    network_log_printf("WAIT","receive_waiting_for_event event=%s result=timeout",network_event_name(event));
	    if (lang() == RUSSIAN) {
            ErrH.Abort("Сервер не отвечает", XERR_USER, event);
        } else {
            ErrH.Abort("Time out of Server's response receiving", XERR_USER, event);
        }
        }
	event_ID = 0;
	offset = next_event_pointer = 0;
	return 0;
}

int InputEventBuffer::next_event() {
	int prev_event_ID = event_ID;
	event_ID = 0;

	//std::cout<<"InputEventBuffer::next_event event_ID:"<<(int)event_ID<<" client_ID:"<<(int)client_ID<<" next_event_pointer:"<<next_event_pointer
	//		 <<" filled_size:"<<filled_size<<std::endl;
	if(next_event_pointer + 2 > filled_size) {
		return 0;
	}
	if(next_event_pointer != tell()) {
		XBuffer str;
		str < "Connection's problems: " <= prev_event_ID < "  "  <= object_ID < "  "  <= next_event_pointer - tell();
		ErrH.Abort(str.GetBuf(), XERR_USER);
	}

	*this > event_size;
	//std::cout<<"event_size:"<<(int)event_size<<std::endl;
	unsigned int new_pointer = next_event_pointer + event_size + 2;
	if(new_pointer > filled_size) {
		set(next_event_pointer);
		return 0;
	}
	next_event_pointer = new_pointer;
	if(!event_size)
		return next_event();

	n_received_events++;

	*this > event_ID;

	//std::cout<<"event_ID:"<<(int)event_ID<<std::endl;
	unsigned char factory_number, ammo_count;
	zCreateObjectQueue* temp;
	if (event_ID == zCREATE_OBJECT_BY_SERVER) {
		std::cout<<"zCREATE_OBJECT_BY_SERVER"<<std::endl;
		//zmod - пакет "создай предмед"
		*this > factory_number > ammo_count;
		body_size = 0;
		if(my_player_body.BirthTime) {
			temp = new zCreateObjectQueue(factory_number, my_player_body.world);
			if (z_create_object_queue) {
				temp->next = z_create_object_queue;
				z_create_object_queue->prev = temp;
			}
			z_create_object_queue = temp;
		}
	}else if(!(event_ID & AUXILIARY_EVENT)) {
		*this > object_ID;
		switch(event_ID) {
			case UPDATE_OBJECT:
				*this  > client_ID > time > x  > y;
				body_size = event_size - 14;
				if(GET_OBJECT_TYPE(object_ID) == NID_VANGER) {
					PlayerData* p = players_list.find(client_ID);
					if(p) {
						p -> x = x;
						p -> y = y;
					}
				}
				delay_time += GLOBAL_CLOCK() - time;
				delay_time_counter++;
				IN_EVENTS_LOG1(UPDATE_OBJECT,object_ID);
				network_log_object_event("IN",event_ID,object_ID,client_ID,time,x,y,-1,body_size,"parsed");
				break;
			case HIDE_OBJECT:
				body_size = 0;
				IN_EVENTS_LOG1(HIDE_OBJECT,object_ID);
				network_log_object_event("IN",event_ID,object_ID,-1,0,-1,-1,-1,body_size,"parsed");
				break;
			case DELETE_OBJECT:
				*this  > client_ID > time;
				body_size = event_size - 10;
				delay_time += GLOBAL_CLOCK() - time;
				delay_time_counter++;
				IN_EVENTS_LOG1(DELETE_OBJECT,object_ID);
				network_log_object_event("IN",event_ID,object_ID,client_ID,time,-1,-1,-1,body_size,"parsed");
				break;
			default:
				ErrH.Abort("Received unknown event",XERR_USER,event_ID);
		}

		if(NON_GLOBAL_OBJECT(object_ID)) {
			if(!enable_transferring) {
				IN_EVENTS_LOG1(Disable_query,object_ID);
				network_log_object_event("IN",event_ID,object_ID,client_ID,time,event_ID == UPDATE_OBJECT ? x : -1,event_ID == UPDATE_OBJECT ? y : -1,-1,body_size,"ignored_disable_transferring");
				ignore_event();
			}
			if(GET_WORLD(object_ID) != CurrentWorld) {
				IN_EVENTS_LOG1(Receive_from_another_world,object_ID);
				network_log_object_event("IN",event_ID,object_ID,client_ID,time,event_ID == UPDATE_OBJECT ? x : -1,event_ID == UPDATE_OBJECT ? y : -1,-1,body_size,"ignored_wrong_world");
				ignore_event();
			}
		}
	} else {
		IN_EVENTS_LOG1(AXILIARY_EVENT,event_ID);
		switch(event_ID){
			case SERVER_TIME:
				//zmod
				z_time_collect();
				*this > time;
				if(enable_transferring) {
					ignore_event();
					if(!lag_averaging_t0.empty()) {
						int dt = (int)(SDL_GetTicks() - lag_averaging_t0.get());
						// if(dt > 100 && dt < 20000) {
							average_lag = dt;
						// }
					}
				}
				body_size = 0;
				network_log_printf("IN","SERVER_TIME server_time=%d average_lag=%d",time,average_lag);
				break;
			case ATTACH_TO_GAME_RESPONSE:
			case GAME_DATA_RESPONSE:
			case SET_WORLD_RESPONSE:
			case RESTORE_CONNECTION_RESPONSE:
			case TOP_LIST_RESPONSE:
			case TOTAL_LIST_OF_PLAYERS_DATA:
			case zGAME_DATA_RESPONSE: //zmod
			case zTIME_RESPONSE: //zmod
				body_size = event_size - 1;
				network_log_printf("IN","%s body_size=%d",network_event_name(event_ID),body_size);
				break;

			case PLAYERS_NAME:
			case PLAYERS_POSITION:
			case PLAYERS_WORLD:
			case PLAYERS_STATUS:
			case PLAYERS_DATA:
			case PLAYERS_RATING:
				network_log_printf("IN","%s body_size=%d",network_event_name(event_ID),event_size - 1);
				players_list.single_parsing(event_ID);
				body_size = 0;
				next_event();
				//ignore_event();
				break;

			case DIRECT_RECEIVING:
				*this > client_ID;
				network_log_printf("IN","DIRECT_RECEIVING creator=%d body_size=%d",client_ID,event_size - 2);
				if(get_byte()) {
					--*this;
					message_dispatcher.receive();
					body_size = 0;
					ignore_event();
				} else {
					body_size = event_size - 2;
				}
				break;

			default:
				ErrH.Abort("Received unknown axiliary event", XERR_USER, event_ID);
		}
	}
	return event_ID ? event_ID : next_event();
}

void InputEventBuffer::ignore_event()
{
	event_ID = 0;
	set(next_event_pointer);
	//std::cout<<"InputEventBuffer::ignore_event next_event_pointer:"<<next_event_pointer<<std::endl;
}
/***********************************************************************
				Some utilites
***********************************************************************/
int connect_to_server(ServerFindChain* p)
{
	network_log_open(p,"connect_to_server");
	network_log_printf("CONNECT","server_game_id=%d",p ? p->game_ID : 0);
	p->connect(main_socket);
	if(main_socket() && identification(main_socket)){
		current_server_addr = *p;

		events_out.clear();
		events_in.reset();

		events_out.begin_event(ATTACH_TO_GAME);
		events_out < current_server_addr.game_ID;
		events_out.end_body();
		events_out.send(1);

		//std::cout<<"ATTACH_TO_GAME_RESPONSE"<<std::endl;
		events_in.receive_waiting_for_event(ATTACH_TO_GAME_RESPONSE);
		//std::cout<<"[ok]"<<std::endl;
		current_server_addr.game_ID = events_in.get_int();
		current_server_addr.configured = events_in.get_byte();
		game_birth_time_offset = events_in.get_dword();
		GlobalStationID = events_in.get_byte();
		for(int i = 0;i < 16;i++)
			object_ID_offsets[i] = events_in.get_word();
		network_log_printf("ATTACH_TO_GAME_RESPONSE","game_id=%d configured=%d game_birth_time=%u GlobalStationID=%d",current_server_addr.game_ID,current_server_addr.configured,game_birth_time_offset,GlobalStationID);
		events_in.ignore_event();

		zGameBirthTime = 0;
		if (zserver_version > 1 || SERVER_VERSION > 1) {
			// zMod second network packet, used as a seed for world activity cycles
			events_in.receive_waiting_for_event(zTIME_RESPONSE);
			zGameBirthTime = events_in.get_int();
			events_in.ignore_event();
		}

		NetworkON = 1;
		number_of_reconnection_attempt = 5;
		network_log_printf("CONNECT_OK","game_id=%d configured=%d GlobalStationID=%d CurrentWorld=%d",current_server_addr.game_ID,current_server_addr.configured,GlobalStationID,CurrentWorld);

		return GlobalStationID;
		}
	NetworkON = 0;
	network_log_printf("CONNECT_FAIL","socket_alive=%d",main_socket());
	if(!main_socket())
		network_log_printf("SOCKET_CLOSED","reason=connect_failed");
	network_log_close("connect_failed");
	return 0;
}
int restore_connection()
{
	if(main_socket())
		return 1;
	//std::cout<<"restore_connection:Connection lost"<<std::endl;
	DOUT("Connection lost");
	network_log_printf("RECONNECT_START","attempts_left=%d",number_of_reconnection_attempt);
	current_server_addr.connect(main_socket);
	if(!main_socket || !identification(main_socket)){
		main_socket.close();
		network_log_printf("SOCKET_CLOSED","reason=reconnect_failed stage=connect_or_identification");
		network_log_printf("RECONNECT_FAIL","stage=connect_or_identification attempts_left=%d",number_of_reconnection_attempt);
		if(number_of_reconnection_attempt-- <= 0)
            {
		    if (lang() == RUSSIAN) {
                ErrH.Abort("Не могу восстановить соединение с Сервером");
            } else {
                ErrH.Abort("Unable to restore connection to Server");
            }
            }
		return 0;
	}
	events_out.clear();
	events_in.reset();
	events_out.begin_event(RESTORE_CONNECTION);
	events_out < current_server_addr.game_ID < (unsigned char)GlobalStationID;
	events_out.end_body();
	events_out.send(1);
	int got = events_in.receive_waiting_for_event(RESTORE_CONNECTION_RESPONSE);
	if(got != RESTORE_CONNECTION_RESPONSE) {
		main_socket.close();
		network_log_printf("SOCKET_CLOSED","reason=reconnect_failed stage=wait_response");
		network_log_printf("RECONNECT_FAIL","stage=wait_response got=%s",network_event_name(got));
		return 0;
	}
	if(events_in.current_body_size() < 1)
		ErrH.Abort("Bad RESTORE_CONNECTION_RESPONSE", XERR_USER, events_in.current_body_size());
	int resp = events_in.get_byte();
	events_in.ignore_event();
	if(!resp) {
		main_socket.close();
		network_log_printf("SOCKET_CLOSED","reason=reconnect_failed stage=server_refused");
		network_log_printf("RESTORE_CONNECTION_RESPONSE","status=%d",resp);
		network_log_printf("RECONNECT_FAIL","stage=server_refused");
		return 0;
	}
	number_of_reconnection_attempt = 5;
	DOUT1("Connection restore",resp);
	network_log_printf("RESTORE_CONNECTION_RESPONSE","status=%d",resp);
	network_log_printf("RECONNECT_OK","game_id=%d GlobalStationID=%d CurrentWorld=%d",current_server_addr.game_ID,GlobalStationID,CurrentWorld);
	return resp;
}
void disconnect_from_server()
{
	network_log_printf("CLOSE_SOCKET","reason=disconnect_from_server");
	events_out.send_simple_query(CLOSE_SOCKET);
	delay(256);
	main_socket.close();
	network_log_printf("SOCKET_CLOSED","reason=disconnect_from_server");
	delay(256);
	events_out.clear();
	events_in.reset();
	network_log_close("disconnect_from_server");
}
void set_time_by_server(int n_measures)
{
	int i;
	int t0,T1,t2;
	double tau = 0;
	double dtau,dtau2 = 0;
	for(i = 0;i < n_measures;i++){
		//zmod
		z_time_init();
		events_out.send_simple_query(SERVER_TIME_QUERY);
		events_in.receive_waiting_for_event(SERVER_TIME);
		t0 = events_in.current_time();
		events_in.ignore_event();
		T1 = GLOBAL_CLOCK();

		//zmod
		z_time_init();
		events_out.send_simple_query(SERVER_TIME_QUERY);
		events_in.receive_waiting_for_event(SERVER_TIME);
		t2 = events_in.current_time();
		events_in.ignore_event();
		dtau = T1 - (t2 + t0)/2;
		tau += dtau;
		dtau2 += sqr(dtau);
		response_time += t2 - t0;
		}
	double N = n_measures;
	global_clock_tau += round(tau /= N);
	response_time /= N*256.;
	average_lag = round(response_time*1000);
	time_synchronization_sigma = sqrt((dtau2 - N*sqr(tau))/N/(N-1))/256.;
	std::cout<<"set_time_by_server time_synchronization_sigma:"<<time_synchronization_sigma
			 <<" average_lag:"<<average_lag<<" t2:"<<t2<<std::endl;
	network_log_printf("LAG","set_time_by_server measures=%d average_lag=%d response_time=%.6f sigma=%.6f",n_measures,average_lag,response_time,time_synchronization_sigma);
}
int set_world(int world,int world_y_size) //znfo - send set_world event
{
	events_out.set_world(world,world_y_size);
	events_out.send(1);
	int got = events_in.receive_waiting_for_event(SET_WORLD_RESPONSE);
	if(got != SET_WORLD_RESPONSE)
        {
	    network_log_printf("SET_WORLD_RESPONSE","requested_world=%d got=%s status=missing",world,network_event_name(got));
	    if (lang() == RUSSIAN) {
            ErrH.Abort("Сервер не отвечает", XERR_USER, SET_WORLD_RESPONSE);
        } else {
            ErrH.Abort("Time out of Server's response receiving", XERR_USER, SET_WORLD_RESPONSE);
        }
        }
	if(events_in.current_body_size() < 2)
		ErrH.Abort("Bad SET_WORLD_RESPONSE", XERR_USER, events_in.current_body_size());
	int resp_world = events_in.get_byte();
	int resp_status = events_in.get_byte();
	if(resp_world != world)
		ErrH.Abort("Incorrect SET_WORLD_RESPONSE", XERR_USER, resp_world);
	events_in.ignore_event();
	set_world_status = resp_status;
	enable_transferring = 1;
	network_log_printf("SET_WORLD_RESPONSE","requested_world=%d response_world=%d status=%d",world,resp_world,resp_status);
	network_log_printf("CONTEXT","enable_transferring changed value=%d CurrentWorld=%d",enable_transferring,CurrentWorld);
	lag_averaging_t0.clear();
	return resp_status;
}
void leave_world()
{
	network_log_printf("OUT","LEAVE_WORLD");
	events_out.send_simple_query(LEAVE_WORLD);
	enable_transferring = 0;
	network_log_printf("CONTEXT","enable_transferring changed value=%d CurrentWorld=%d",enable_transferring,CurrentWorld);
}
void total_players_data_list_query()
{
	total_players_data_list_query_pending = 1;
	total_players_data_list_query_last_time = SDL_GetTicks();
	network_log_printf("OUT","TOTAL_PLAYERS_DATA_QUERY blocking=1");
	events_out.send_simple_query(TOTAL_PLAYERS_DATA_QUERY);
	events_in.receive_waiting_for_event(TOTAL_LIST_OF_PLAYERS_DATA);
	players_list.parsing_total_body_query();
	events_in.ignore_event();
}

void request_total_players_data_list_query_async()
{
	unsigned int now = SDL_GetTicks();
	if(total_players_data_list_query_pending && now - total_players_data_list_query_last_time < TOTAL_PLAYERS_DATA_LIST_QUERY_RETRY_MS)
		return;

	total_players_data_list_query_pending = 1;
	total_players_data_list_query_last_time = now;
	network_log_printf("OUT","TOTAL_PLAYERS_DATA_QUERY blocking=0");
	events_out.send_simple_query(TOTAL_PLAYERS_DATA_QUERY);
}
void send_player_body(PlayerBody& body)
{
	if(my_player_data)
		my_player_data -> body = body;
	events_out.set_player_body(body);
	events_out.send(1);
}

void network_analysis(XBuffer& out,int integral)
{
	out.init();
	out.SetDigits(3);
	out < "Network's Statistics\n";
	out < (!non_blocking_mode ? "Blocking send()\n" :  "Non-blocking send()\n");
#ifdef EVENTS_LOG
	out < "Net events is being logged\n";
#endif
	out < "Set world status: " <= set_world_status < "\n";
	out < "Players:\n";
	PlayerData* p = players_list.first();
	while(p){
		out <= p -> client_ID < "\ts:" <= p -> status < "\tw:" <= p -> world < "\tx:" <= p -> x < "\ty:" <= p -> y < "\tk:" <= p -> body.kills < "\td:" <= p -> body.deaths < "\t";
		if(p -> name)
			out < p -> name < "\t";
		out < "\n";
		p = p -> next;
		}

	out < "Chat:\n";
	MessageElement* pm = message_dispatcher.last();
	while(pm){
		out < pm -> message < "\n";
		if(pm == message_dispatcher.first())
			break;
		pm = (MessageElement*)pm -> prev;
		}

	if(!integral){
		double dt = (double)(SDL_GetTicks() - (int)network_clock)/1000;
		double df = frame - network_frame;
		out < "Rate: " <= df/dt < " FPS\n";
		out < "Time synchronization, lag: " <= response_time < " Sec,  Sigma: " <= time_synchronization_sigma < " Sec\n";
		//out < "Average lag of events (C->S->C): " <= (double)delay_time/(delay_time_counter*256) < " Sec\n";
		out < "Average lag of events (C->S->C): " <= average_lag < " mSec\n";
		out < "Send:  " <= round(n_sended_events/dt) < " EPS,  " <= round(n_sended_bytes/dt) < " BPS,  " <= round(n_sended_events/df) < " EPF,  " <= round(n_sended_bytes/df) < " BPF\n";
		out < "Receive: " <= round(n_received_events/dt) < " EPS,  " <= round(n_received_bytes/dt) < " BPS,  " <= round(n_received_events/df) < " EPF,  " <= round(n_received_bytes/df) < " BPF\n";
		out < "Total send: " <= n_sended_bytes_global < "\n";
		out < "Total receive: " <= n_received_bytes_global < "\n";

		if(now_unsent_size)
			out < "Unsent size: " <= now_unsent_size < "\n";

		network_clock = SDL_GetTicks();
		network_frame = frame;
		n_sended_events_global += n_sended_events;
		n_sended_bytes_global += n_sended_bytes;
		n_received_events_global += n_received_events;
		n_received_bytes_global += n_received_bytes;
		delay_time_global += delay_time;
		delay_time_counter_global += delay_time_counter;
		if(global_unsent_size < now_unsent_size)
			global_unsent_size = now_unsent_size;
		now_unsent_size = n_sended_events = n_sended_bytes = n_received_events = n_received_bytes = delay_time = delay_time_counter = 0;
		}
	else{
		out < "Frames: "        <= frame < "\n";
		out < "Time: " <= (double)GLOBAL_CLOCK()/256. < " sec\n";
		double dt = (double)(SDL_GetTicks() - (int)network_clock_global)/1000;
		double df = frame - network_frame_global;
		out < "Rate: " <= df/dt < " FPS\n";
		out < "Time synchronization, lag: " <= response_time < " Sec,  Sigma: " <= time_synchronization_sigma < " Sec\n";
		//out < "Average delay of events (C->S->C): " <= (double)delay_time_global/(delay_time_counter_global*256) < " Sec\n";
		out < "Average lag of events (C->S->C): " <= average_lag < " mSec\n";
		out < "Send:   " <= n_sended_events_global/dt < " EPS,  " <= n_sended_bytes_global/dt < " BPS,  " <= n_sended_events_global/df < " EPF,  " <= n_sended_bytes_global/df < " BPF\n";
		out < "Receive: " <= n_received_events_global/dt < " EPS,  " <= n_received_bytes_global/dt < " BPS,  " <= n_received_events_global/df < " EPF,  " <= n_received_bytes_global/df < " BPF\n";
		if(global_unsent_size)
			out < "Global unsent size: " <= global_unsent_size < "\n";
		}

//zmod
	out < "\n";
	out < "zTime: " <= z_time < " / " <= z_time_avg < "\n";
//#ifdef ZMOD_BETA
//	out < "zCycle		: " <= zMod_cycle < "\n";
//	out < "zFlood		: " <= zMod_flood_level_delta < "\n";
//	out < "zBirthTime	: " <= zGameBirthTime < "\n";
//	out < "InitialRND	: " <= (unsigned int)my_server_data.InitialRND < "\n";
//#endif

	if(network_clock_global == -1){
		network_clock_global = SDL_GetTicks();
		network_frame_global = frame;
		n_sended_events_global = n_sended_bytes_global = n_received_events_global = n_received_bytes_global = delay_time_global = 0;
		}
}
void short_network_analysis(XBuffer& out)
{
	out.init();
	out.SetDigits(3);
	out < "Network's Statistics\n";
	out < "Time synchronization lag: " <= response_time < " Sec\n";
	out < "Time synchronization sigma: " <= time_synchronization_sigma < " Sec\n";
	double dt = (double)(SDL_GetTicks() - (int)network_clock)/1000;
	double df = frame - network_frame;
	out < (main_socket() ? "Connection ok\n" : "Connection lost\n");
	out < "Rate: " <= df/dt < " FPS\n";
	//if(delay_time_counter)
	//out < "Average lag of events (C->S->C): " <= (double)delay_time/(delay_time_counter*256) < " Sec\n";
	out < "Average lag of events (C->S->C): " <= average_lag < " mSec\n";
	out < "Send:  " <= round(n_sended_bytes/dt) < " BPS\n";
	out < "Receive: " <= round(n_received_bytes/dt) < " BPS\n";
	out < "Total send: " <= n_sended_bytes_global < "\n";
	out < "Total receive: " <= n_received_bytes_global < "\n";
	if(now_unsent_size)
		out < "Unsent size: " <= now_unsent_size < "\n";

//zmod
	out < "\n";
	out < "zTime: " <= z_time < " / " <= z_time_avg < "\n";
//#ifdef ZMOD_BETA
//	out < "zCycle		: " <= zMod_cycle < "\n";
//	out < "zFlood		: " <= zMod_flood_level_delta < "\n";
//	out < "zBirthTime	: " <= zGameBirthTime < "\n";
//	out < "InitialRND	: " <= (unsigned int)my_server_data.InitialRND < "\n";
//#endif

	network_clock = SDL_GetTicks();
	network_frame = frame;
	n_sended_events_global += n_sended_events;
	n_sended_bytes_global += n_sended_bytes;
	n_received_events_global += n_received_events;
	n_received_bytes_global += n_received_bytes;
	delay_time_global += delay_time;
	delay_time_counter_global += delay_time_counter;
	if(global_unsent_size < now_unsent_size)
		global_unsent_size = now_unsent_size;
	now_unsent_size = n_sended_events = n_sended_bytes = n_received_events = n_received_bytes = delay_time = delay_time_counter = 0;

	if(network_clock_global == -1){
		network_clock_global = SDL_GetTicks();
		network_frame_global = frame;
		n_sended_events_global = n_sended_bytes_global = n_received_events_global = n_received_bytes_global = delay_time_global = 0;
		}
}


void delay(int msec)
{
	START_TIMER(msec);
	while(CHECK_TIMER());
}

/***********************************************************************
				Server's Data
***********************************************************************/
ServerData::ServerData()
{
	time_t _time_;
	InitialRND = (time(&_time_) | 1)*6386891;
///	InitialRND = 83838383;
	GameType = PASSEMBLOSS; //VAN_WAR; //zmod beta
}
//zmod
int is_current_server_configured(char* name, ServerData* data, zServerData* zdata)
{
	if(current_server_addr.configured){
		events_out.send_simple_query(GET_GAME_DATA);
		if(events_in.receive_waiting_for_event(GAME_DATA_RESPONSE)){
			if(events_in.current_body_size()){
				events_in > name;
				events_in.read(data,sizeof(ServerData));
				MP_GAME = data -> GameType;
				events_in.ignore_event();
				//---------------------------zMod fixed net packet zGAME_DATA_RESPONSE
				if (zserver_version>1) {
					if (events_in.receive_waiting_for_event(zGAME_DATA_RESPONSE)) {
						if(events_in.current_body_size()) {
							zdata->mod_id = events_in.get_byte();
							events_in.ignore_event();
							return 1;
						}
						events_in.ignore_event();
						return 0;
					}
				}
				//---------------------------zMod net packet zGAME_DATA_RESPONSE
				return 1;
            }
            events_in.ignore_event();
		    return 0;
			}
		}
	return 0;
}
//zmod
int send_server_data(char* name, ServerData* data, zServerData* zdata)
{
	MP_GAME = data -> GameType;
	if(current_server_addr.configured){
		events_out.send_simple_query(GET_GAME_DATA);
		if(events_in.receive_waiting_for_event(GAME_DATA_RESPONSE)){
			if(events_in.current_body_size()){
				events_in > name;
				events_in.read(data,sizeof(ServerData));
				MP_GAME = data -> GameType;
				events_in.ignore_event();
				//---------------------------zMod fixed net packet zGAME_DATA_RESPONSE
				if (zserver_version>1) {
					if (events_in.receive_waiting_for_event(zGAME_DATA_RESPONSE)) {
						if(events_in.current_body_size()) {
							zdata->mod_id = events_in.get_byte();
							events_in.ignore_event();
							return 1;
						}
						events_in.ignore_event();
						return 0;
					}
				}
				//---------------------------zMod net packet zGAME_DATA_RESPONSE
				return 0;//zmod - MAYBE NOT NEEDED
				}
			events_in.ignore_event();
			}
		return 0;
		}

	events_out.begin_server_data();
	events_out < name < char(0);
	events_out.write(data,sizeof(ServerData));
	events_out.end_body();
	events_out.send_simple_query(GET_GAME_DATA);
	if(!events_in.receive_waiting_for_event(GAME_DATA_RESPONSE) || !events_in.current_body_size())
		ErrH.Abort("Network error: unable to configure server");
	char name_tmp[256];
	ServerData data_tmp;
	events_in > name_tmp;
	events_in.read(&data_tmp,sizeof(ServerData));
	events_in.ignore_event();
	//---------------------------zMod fixed net packet zGAME_DATA_RESPONSE
	if (zserver_version>1) {
		if(!events_in.receive_waiting_for_event(zGAME_DATA_RESPONSE) || !events_in.current_body_size())
			ErrH.Abort("Network error: unable to configure server");
		zdata->mod_id = events_in.get_byte();
		events_in.ignore_event();
	}
	//---------------------------zMod net packet zGAME_DATA_RESPONSE
	current_server_addr.configured = 1;
	if(strcmp(name,name_tmp) || memcmp(data,&data_tmp,sizeof(ServerData))){
		strcpy(name,name_tmp);
		memcpy(data,&data_tmp,sizeof(ServerData));
		return 0;
		}
	return 1;
}
void void_network_quant()
{
	events_in.receive();
	while(events_in.current_event()) {
		events_in.ignore_event();
		events_in.next_event();
	}
}
/*******************************************************************************
				Player's List
*******************************************************************************/
PlayerData::PlayerData(int client_id)
{
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
	for(int i = 0;i < MAX_ACTIVE_SLOT;i++){
		SlotNetID[i] = 0;
		SlotStuffNetID[i] = 0;
		SlotFireCount[i] = 0;
			}
}

PlayerData* PlayersList::find_any(int client_id)
{
	PlayerData* p = first();
	while(p){
		if(p -> client_ID == client_id)
			return p;
		p = p -> next;
	}
	return NULL;
}

PlayerData* PlayersList::find(int client_id)
{
	PlayerData* p = first();
	while(p){
		if(p -> client_ID == client_id && p -> status != FINISHED_STATUS){
			return p;
			}
		p = p -> next;
		}
	p = new PlayerData(client_id);
	append(p);
	return p;
}

static void assign_player_name(PlayerData* p,const char* name)
{
	if(!p -> name){
		p -> name = new char[strlen(name) + 1];
		strcpy(p -> name, name);
	}else{
		if(strcmp(p -> name,name)){
			delete[] p -> name;
			p -> name = new char[strlen(name) + 1];
			strcpy(p -> name, name);
		}
	}
}

void PlayersList::single_parsing(int event_ID)
{
	PlayerData* p = find(events_in.get_byte());
	switch(event_ID){
		case PLAYERS_NAME:{
			char name[257];
			events_in > name;
			assign_player_name(p,name);
			}
			break;
		case PLAYERS_POSITION:
			p -> x = events_in.get_short();
			p -> y = events_in.get_short();
			break;
		case PLAYERS_WORLD:
			p -> world = events_in.get_byte();
			break;
		case PLAYERS_STATUS:
			p -> status = events_in.get_byte();
			if(p -> status == FINISHED_STATUS)
				PLAYER_REMOVAL_EVENT(p);
			break;
		case PLAYERS_DATA:
			events_in.read(&(p -> body),sizeof(PlayerBody));
			break;
		case PLAYERS_RATING:
			p -> body.rating = (float)events_in.get_float();
			break;
		}
}
unsigned int PlayersList::get_team_mask(int color)
{
	unsigned int mask = 0;
	PlayerData* p = first();
	while(p){
		if(p -> body.color == color && p -> status != FINISHED_STATUS)
			mask |= 1 << (p -> client_ID - 1);
		p = p -> next;
		}
	return mask;
}
void PlayersList::parsing_total_body_query()
{
	unsigned char client_ID,status,world;
	short x,y;
	char name[257];
	PlayerData* p;
	delete_all();
	my_player_data = 0;
	int num = events_in.get_byte();
	for(int i = 0;i < num;i++){
		events_in > client_ID > status > world > x > y > name;
		p = new PlayerData(client_ID);
		append(p);
		p -> status = status;
		p -> world = world;
		p -> x = x;
		p -> y = y;
		events_in.read(&p -> body,sizeof(PlayerBody));
		if(!p -> name){
			p -> name = new char[strlen(name) + 1];
			strcpy(p -> name, name);
			}
		else{
			if(strcmp(p -> name,name)){
				delete p -> name;
				p -> name = new char[strlen(name) + 1];
				strcpy(p -> name, name);
				}
			}
			if(p -> client_ID == GlobalStationID)
				my_player_data = p;
			}
	total_players_data_list_query_pending = 0;
}

void PlayersList::merge_total_body_query()
{
	unsigned char client_ID,status,world;
	short x,y;
	char name[257];
	PlayerBody body;
	PlayerData* p;
	PlayerData* current_player_data = NULL;
	int num = events_in.get_byte();

	p = first();
	while(p){
		p -> GamingFlag = 0;
		p = (PlayerData*)(p -> next);
	}

	for(int i = 0;i < num;i++){
		events_in > client_ID > status > world > x > y > name;
		events_in.read(&body,sizeof(PlayerBody));

		p = NULL;
		if(status != FINISHED_STATUS){
			PlayerData* n = first();
			while(n){
				if(!n -> GamingFlag && n -> client_ID == client_ID && n -> status != FINISHED_STATUS){
					p = n;
					break;
				}
				n = (PlayerData*)(n -> next);
			}
		}else{
			PlayerData* candidate = NULL;
			PlayerData* n = first();
			while(n){
				if(!n -> GamingFlag && n -> client_ID == client_ID){
					if(n -> status == FINISHED_STATUS){
						p = n;
						break;
					}
					if(!candidate)
						candidate = n;
				}
				n = (PlayerData*)(n -> next);
			}
			if(!p)
				p = candidate;
		}

		if(!p){
			p = new PlayerData(client_ID);
			append(p);
		}

		int prev_status = p -> status;
		p -> GamingFlag = 1;
		p -> status = status;
		p -> world = world;
		p -> x = x;
		p -> y = y;
		assign_player_name(p,name);
		p -> body = body;

		if(prev_status != FINISHED_STATUS && p -> status == FINISHED_STATUS)
			PLAYER_REMOVAL_EVENT(p);

		if(p -> client_ID == GlobalStationID){
			if(p -> status != FINISHED_STATUS || !current_player_data)
				current_player_data = p;
		}
	}

	if(current_player_data)
		my_player_data = current_player_data;

	total_players_data_list_query_pending = 0;
}

/*******************************************************************************
			Chat Tool
*******************************************************************************/
MessageElement::MessageElement(const char* player_name, char* msg,int col)
{
	message = new char[strlen(player_name) + strlen(msg) + 3];
	strcpy(message,player_name);
	strcat(message,": ");
	strcat(message,msg);
	color = col;
	//zmod
    time = SDL_GetTicks();
}

void MessageDispatcher::send(char* message,int mode,int parameter)
{
	unsigned int cors;
	switch(mode){
		case MESSAGE_FOR_ALL:
			cors = 0xffffffff & ~(1 << (GlobalStationID - 1));
			break;
		case MESSAGE_FOR_TEAM:
			cors = players_list.get_team_mask(parameter) & ~(1 << (GlobalStationID - 1));
			break;
		case MESSAGE_FOR_PLAYER:
			cors = 1 << (parameter - 1);
			break;
		}
	events_out.begin_direct_send(cors);
	events_out < message < char(0);
	events_out.end_body();
	events_out.send(1);

	MessageElement* p = new MessageElement(CurPlayerName, message, my_player_body.color);
	AddElement(p);
	if(ListSize > max_number_of_messages){
		RemoveElement(p = first());
		delete p;
		}
    //zmod
	new_messages = 1;
}

void IncomingChatMessage(void);
void MessageDispatcher::receive()
{
	char msg[512];
	events_in > msg;
	PlayerData* p = players_list.find(events_in.current_creator());
	MessageElement* pm = new MessageElement(p ? p -> name : (char *)"Unknown", msg, p -> body.color);
	AddElement(pm);
	if(ListSize > max_number_of_messages){
		RemoveElement(pm = first());
		delete pm;
		}
#ifdef _ROAD_
	SOUND_BEEP();
    //zmod
    //IncomingChatMessage();
#endif
	//zmod
    new_messages = 1;
}

/********************************************************************
		Top List
********************************************************************/
RatingData::RatingData(char* _name,float _rating)
{
	name = new char[strlen(_name) + 1];
	strcpy(name,_name);
	rating = _rating;
	next = prev = 0;
	list = 0;
}
RatingData::~RatingData()
{
	delete name;
}
TopList::TopList(int MP_game)
{
	events_out.send_simple_query(TOP_LIST_QUERY,MP_game);
	events_in.receive_waiting_for_event(TOP_LIST_RESPONSE);
	char name[257];
	float rating;
	RatingData* p;
	events_in.get_byte();
	int num = events_in.get_byte();
	while(num--){
		events_in > name > rating;
		p = new RatingData(name,rating);
		append(p);
		}
	events_in.ignore_event();
}
TopList::~TopList()
{
	delete_all();
}
/********************************************************************
	Network's Monitoring
********************************************************************/
#ifdef _FOUT_
double _sigma_;
double single_measurement(int size,int delay,int N)
{
	static unsigned char buffer[16000];
	int time = 0;
	int sent = 0;
	int recv = 0;
	int recv_time;
	double sum_time = 0;
	double sum_time2 = 0;
	XCon < "Measurement: " <= size < "\t" <= delay < "\t" <= N < "\n";
	if((size -= 8) < 0)
		size = 0;
	for(;recv < N;){
		if(time < SDL_GetTicks()){
			events_out.begin_direct_send(1 << GlobalStationID - 1);
			events_out < char(0) < SDL_GetTicks();
			events_out.write(buffer,size);
			events_out.end_body();
			events_out.send(1);
			time = SDL_GetTicks() + delay;
			sent++;
			}
		events_in.receive();
		while(events_in.current_event()){
			if(events_in.current_event() == DIRECT_RECEIVING){
				events_in > recv_time;
				sum_time += SDL_GetTicks() - recv_time;
				sum_time2 += sqr((double)SDL_GetTicks() - recv_time);
				recv++;
				}
			events_in.ignore_event();
			}
		XCon <= sent < "\t" <= recv < "\r";
		}
	double avr_time = sum_time/N;
	_sigma_ = sqrt((sum_time2 - N*sqr(avr_time))/(N*(N-1)));
	XCon < "\nResult: " <= avr_time < "\t" <= _sigma_ < "\n";
	fout < "Measurement: " <= size < "\t" <= delay < "\t" <= N < "\n";
	fout < "\nResult: " <= avr_time < "\t" <= _sigma_ < "\n";
	return avr_time;
}
void monitoring_of_size()
{
	single_measurement(8,1000,200);
	single_measurement(50,1000,200);
	single_measurement(150,1000,200);
	single_measurement(400,1000,200);
	single_measurement(1000,1000,200);

	single_measurement(400,200,200);
	single_measurement(400,800,200);
	single_measurement(400,1600,200);
	single_measurement(400,3000,200);
}
#endif

int calc_CRC(unsigned char* p, int size)
{
	int VAL = 276859467;
	int sum = 0;
	while(size-- > 0){
		sum += VAL ^ *(p++);
		VAL ^= VAL >> 3;
		VAL ^= VAL << 28;
		VAL &= 0x7FFFFFFF;
		}
	return sum;
}
struct MemTestBlock {
	int size;
	unsigned char* ptr;
	int CRC;
	MemTestBlock* next;
	MemTestBlock* prev;
	XTList<MemTestBlock>* list;

	MemTestBlock(unsigned char* p, int _size,int _CRC){ ptr = p; size = _size; CRC = _CRC; next = prev = 0; list = 0; }
};

XTList<MemTestBlock> mem_test_list;

int test_block(unsigned char* ptr, int size)
{
	int CRC = calc_CRC(ptr,size);
	MemTestBlock* p = mem_test_list.first();
	while(p){
		if(ptr == p -> ptr && size == p -> size)
			return CRC != p -> CRC;
		p = p -> next;
		}
	mem_test_list.append(new MemTestBlock(ptr,size,CRC));
	return 0;
}



//===================================================================================

