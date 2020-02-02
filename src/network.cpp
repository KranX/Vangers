//zmod
#include "zmod_client.h"

#ifdef _NetRally_
#include "NetRally.h"
#else
#include "global.h"
#include "3d/3d_math.h"
#include "sound/hsound.h"
#endif

#include "xsocket.h"
#ifndef _WIN32
#include <arpa/inet.h> // ntohl() FIXME: remove
#endif
#include "network.h"

extern int MP_GAME;
extern XStream fout;
extern int frame;
extern int GlobalExit;

#define CLIENT_VERSION	1
#define SERVER_VERSION	1

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

MessageDispatcher message_dispatcher;

XSocket main_socket;
ServerFindChain current_server_addr;
int number_of_reconnection_attempt = 5;

int object_ID_offsets[16];



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
#if defined(RUSSIAN_VERSION) && !defined(GERMAN_VERSION)
		//CP866 Новая игра на
		const unsigned char new_game_on[] = {0x8D, 0xAE, 0xA2, 0xA0, 0xEF, 0x20, 0xA8, 0xA3, 0xE0, 0xA0, 0x20, 0xAD, 0xA0, 0x20};
		str_buf < (const char *)new_game_on;
#else
		str_buf < "New Game on ";
#endif
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
	START_TIMER(60*CLOCKS_PER_SEC);
	const char* request_str = "Vivat Sicher, Rock'n'Roll forever!!!";
	strcpy(string, request_str);
	string[strlen(string) + 1] = CLIENT_VERSION;
	socket.send(string,strlen(string) + 2);
	while(CHECK_TIMER())
		if((len = socket.receive(string, 255, 1000)) != 0 && !strcmp(string,"Enter, my son, please...")){
			if(!SERVER_VERSION || (len > strlen(string) + 1 && string[strlen(string) + 1] == SERVER_VERSION))
				identificated = 1;
			break;
			}
	if(!identificated) {
		std::cout<<"Network:identificated is wrong! SV:"<<SERVER_VERSION<<" SV2:"<<(int)string[strlen(string)+1]<<std::endl;
		socket.close();
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

	START_TIMER(60*CLOCKS_PER_SEC);
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

int ServersList::find_servers(int bc_port)
{
	char buffer[300];
	int IP,port, sent_size;
	char* name;
	ServerFindChain* p;
	XSocket udp_sock;
	udp_sock.openUDP(bc_port);
	clear_states();
	START_TIMER(5*CLOCKS_PER_SEC);
	while(CHECK_TIMER()){
		sent_size = udp_sock.receivefrom(buffer,256);
		if(buffer[0] != 'K' || buffer[1] != 'D'){
			continue;
			}
		IP = *((int*)(buffer + 2));
		port = *((unsigned short*)(buffer + 6));
		if(sent_size > 8){
			buffer[sent_size] = 0;
			name = strupr(buffer + 8);
			}
		else
			name = 0;

		p = first();
		while(p){
			if(p -> IP == IP && p -> port == port)
				break;
			p = p -> next;
			}
		if(!p)
			talk_to_server(IP,port,name);
		}

#ifdef _DEBUG
	if(host_name)
		talk_to_server(0,host_port,host_name);
#endif

	return size();
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
}
void OutputEventBuffer::set_position(int x,int y,int y_half_size_of_screen)
{
	n_event++;
	*this < short(7) < (unsigned char)SET_POSITION < (unsigned short)x < (unsigned short)y < (unsigned short)y_half_size_of_screen;
	OUT_EVENTS_LOG1(SET_POSITION,y);
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
}
void OutputEventBuffer::begin_event(int event_ID)
{
	if(pointer_to_size_of_event != -1)
		ErrH.Abort("There wasn't an end of event");
	pointer_to_size_of_event = offset;
	*this < short(0) < (unsigned char)event_ID;
}
void OutputEventBuffer::end_body()
{
	int i;
	if(pointer_to_size_of_event < 0)
		ErrH.Abort("There wasn't a beginning of event");

	//std::cout<<"OutputEventBuffer::end_body size:"<<tell() - pointer_to_size_of_event - sizeof(short int)<<std::endl;
	*(short*)(address() + pointer_to_size_of_event) = tell() - pointer_to_size_of_event - sizeof(short int);

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
	return send(1,sock);
}
int OutputEventBuffer::send_simple_query(int event, unsigned char data,XSocket& sock)
{
	n_event++;
	*this < short(2) < (unsigned char)event < data;
	OUT_EVENTS_LOG1(send_simple_query,event);
	return send(1,sock);
}
int OutputEventBuffer::send_simple_query(int event, unsigned char data1, unsigned char data2,XSocket& sock)
{
	n_event++;
	*this < short(3) < (unsigned char)event < data1 < data2;
	OUT_EVENTS_LOG1(send_simple_query,event);
	return send(1,sock);
}
void OutputEventBuffer::set_player_body(PlayerBody& body)
{
	n_event++;
	*this < short(1 + sizeof(PlayerBody)) < (unsigned char)SET_PLAYER_DATA;
	write(&body,sizeof(PlayerBody));
	OUT_EVENTS_LOG(SET_PLAYER_DATA);
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
	START_TIMER(10*CLOCKS_PER_SEC);
	while(current_event() || CHECK_TIMER()) {
		do {
			//std::cout<<"current_event:"<<(int)current_event()<<" clock:"<<clock()<<" _end_time_:"<<_end_time_<<std::endl;
			if(current_event() == event) {
				//std::cout<<"ok"<<std::endl;
				int size = event_size + 2;
				int prefix = next_event_pointer - size;
				if(prefix > 0) {
					memmove(buf + size,buf,filled_size);
					memmove(buf,buf + size + prefix,size);
					memmove(buf + size + prefix,buf + 2*size + prefix,filled_size - size - prefix);
					//offset = next_event_pointer = size - body_size;
					offset = next_event_pointer = 0;
					return next_event();
				}
				return event;
			}
			ignore_event();
		} while(next_event());

		receive(sock,1);
	}
	if(!skip_if_aint)
#if defined(RUSSIAN_VERSION) && !defined(GERMAN_VERSION)
		ErrH.Abort("Сервер не отвечает", XERR_USER, event);
#else
		ErrH.Abort("Time out of Server's response receiving", XERR_USER, event);
#endif
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
					p -> x = x; p -> y = y;
				}
				delay_time += GLOBAL_CLOCK() - time;
				delay_time_counter++;
				IN_EVENTS_LOG1(UPDATE_OBJECT,object_ID);
				break;
			case HIDE_OBJECT:
				body_size = 0;
				IN_EVENTS_LOG1(HIDE_OBJECT,object_ID);
				break;
			case DELETE_OBJECT:
				*this  > client_ID > time;
				body_size = event_size - 10;
				delay_time += GLOBAL_CLOCK() - time;
				delay_time_counter++;
				IN_EVENTS_LOG1(DELETE_OBJECT,object_ID);
				break;
			default:
				ErrH.Abort("Received unknown event",XERR_USER,event_ID);
		}

		if(NON_GLOBAL_OBJECT(object_ID)) {
			if(!enable_transferring) {
				IN_EVENTS_LOG1(Disable_query,object_ID);
				ignore_event();
			}
			if(GET_WORLD(object_ID) != CurrentWorld) {
				IN_EVENTS_LOG1(Receive_from_another_world,object_ID);
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
						if(dt > 100 && dt < 20000) {
							average_lag = (dt + average_lag*3) >> 2;
						}
					}
				}
				body_size = 0;
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
				break;

			case PLAYERS_NAME:
			case PLAYERS_POSITION:
			case PLAYERS_WORLD:
			case PLAYERS_STATUS:
			case PLAYERS_DATA:
			case PLAYERS_RATING:
				players_list.single_parsing(event_ID);
				body_size = 0;
				next_event();
				//ignore_event();
				break;

			case DIRECT_RECEIVING:
				*this > client_ID;
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
		events_in.ignore_event();

		zGameBirthTime = 0;
		if (zserver_version > 1) {
			//std::cout<<"zTIME_RESPONSE"<<std::endl;
			events_in.receive_waiting_for_event(zTIME_RESPONSE); //ZMOD second network packet
			//std::cout<<"[ok]"<<std::endl;
			zGameBirthTime = events_in.get_int();
			events_in.ignore_event();
		}

		NetworkON = 1;
		number_of_reconnection_attempt = 5;

		return GlobalStationID;
		}
	NetworkON = 0;
	return 0;
}
int restore_connection()
{
	if(main_socket())
		return 1;
	//std::cout<<"restore_connection:Connection lost"<<std::endl;
	DOUT("Connection lost");
	current_server_addr.connect(main_socket);
	if(!main_socket){
		if(number_of_reconnection_attempt-- <= 0)
#if defined(RUSSIAN_VERSION) && !defined(GERMAN_VERSION)
			ErrH.Abort("Не могу восстановить соединение с Сервером");
#else
			ErrH.Abort("Unable to restore connection to Server");
#endif
		return 0;
	}
	number_of_reconnection_attempt = 5;
	events_out.clear();
	events_in.reset();
	events_out.begin_event(RESTORE_CONNECTION);
	events_out < current_server_addr.game_ID < (unsigned char)GlobalStationID;
	events_out.end_body();
	events_out.send(1);
	events_in.receive_waiting_for_event(RESTORE_CONNECTION_RESPONSE);
	int resp = events_in.get_byte();
	events_in.ignore_event();
	DOUT1("Connection restore",resp);
	return resp;
}
void disconnect_from_server()
{
	events_out.send_simple_query(CLOSE_SOCKET);
	delay(256*(CLOCKS_PER_SEC/1000));
	main_socket.close();
	delay(256*(CLOCKS_PER_SEC/1000));
	events_out.clear();
	events_in.reset();
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
}
int set_world(int world,int world_y_size) //znfo - send set_world event
{
	events_out.set_world(world,world_y_size);
	events_out.send(1);
	events_in.receive_waiting_for_event(SET_WORLD_RESPONSE);
	events_in.get_byte(); //resp_world
	int resp_status = events_in.get_byte();
	events_in.ignore_event();
	set_world_status = resp_status;
	enable_transferring = 1;
	lag_averaging_t0.clear();
	return resp_status;
}
void leave_world()
{
	events_out.send_simple_query(LEAVE_WORLD);
	enable_transferring = 0;
}
void total_players_data_list_query()
{
	events_out.send_simple_query(TOTAL_PLAYERS_DATA_QUERY);
	events_in.receive_waiting_for_event(TOTAL_LIST_OF_PLAYERS_DATA);
	players_list.parsing_total_body_query();
	events_in.ignore_event();
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
	uvsPoint = NULL;
	for(int i = 0;i < MAX_ACTIVE_SLOT;i++){
		SlotNetID[i] = 0;
		SlotStuffNetID[i] = 0;
		SlotFireCount[i] = 0;
		}
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
void PlayersList::single_parsing(int event_ID)
{
	PlayerData* p = find(events_in.get_byte());
	switch(event_ID){
		case PLAYERS_NAME:{
			char name[257];
			events_in > name;
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

