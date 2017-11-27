#ifndef _XSOCKET_H
#define _XSOCKET_H

#include "global.h"
#include "SDL_net.h"

int XSocketInit(int ErrHUsed = 1);
int XSocketGetErrorStatus();
char* XSocketGetLastError();

// extern char XSocketLocalHostName[257];
extern IPaddress XSocketLocalHostADDR; // used in network.cpp
extern IPaddress XSocketLocalHostExternADDR; // used in iscreen/iscr_fnc.cpp
// extern int XSocketInitializationOK;

char* get_name_by_addr(int IP);

#define INVALID_SOCKET  NULL
#define SOCKET_ERROR    (-1)
#define SEND_BROADCAST  (unsigned int)0xffffffff

#define XS_WAITING_TIME 120000

class XSocket
{
	int ErrHUsed;
	TCPsocket tcpSock;
	UDPsocket udpSock;
	SDLNet_SocketSet socketSet;
	IPaddress addr;

	public:
		XSocket();
		~XSocket();

		XSocket(XSocket& donor); // transfers socket ownership, donor dies.
		XSocket& operator = (XSocket& donor); // transfers socket ownership, donor dies.

		int open(int IP,int port);
		int open(char* name,int port);

		// socks4/5 operation is allowed by using FreeCap or tsocks wrappers.
// 		int open_by_socks5(char* name,int port,char* proxy_name,int proxy_port = 1080);

		int openUDP(int port);
		int open_broadcast(int port);
		void close();

		int listen(int port);
		XSocket accept();

		void set_ErrHUsing(int enable_using);
		int set_nonblocking_mode(int enable_nonblocking);

		int check_socket(int ms_time = 0);
		int check_readability(int ms_time = 0);
		int check_writeability(int ms_time = 0);

		int send(const char* buffer, int size);
		int receive(char* buffer, int size_of_buffer, int ms_time = 0);

		int sendto(const char* buffer, int size, unsigned int IP = SEND_BROADCAST);
		int receivefrom(char* buffer, int size_of_buffer, int ms_time = 0);

		int operator! (){ return tcpSock == INVALID_SOCKET && udpSock == INVALID_SOCKET; }
		int operator() (){ return tcpSock != INVALID_SOCKET || udpSock != INVALID_SOCKET; }

	private:
		int tcp_open();
};

// For some reason, container classes were defined in xsocket include in original V.
#include "xcontainers.h"

#endif
