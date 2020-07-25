#ifndef _XSOCKET_H
#define _XSOCKET_H

#include "xglobal.h"
#include "SDL_net.h"

#include "xcontainers.h"

int XSocketInit(int ErrHUsed = 1);

// extern char XSocketLocalHostName[257];
extern IPaddress XSocketLocalHostADDR; // used in network.cpp
extern IPaddress XSocketLocalHostExternADDR; // used in iscreen/iscr_fnc.cpp

#define INVALID_SOCKET  NULL
#define SOCKET_ERROR    (-1)

class XSocket
{
	int ErrHUsed;
	TCPsocket tcpSock;
	SDLNet_SocketSet socketSet;
	public:
		IPaddress addr;

		XSocket();
		~XSocket();

		XSocket(XSocket& donor); // transfers socket ownership, donor dies.
		XSocket& operator = (XSocket& donor); // transfers socket ownership, donor dies.

		int open(int IP,int port);
		int open(char* name,int port);
		void close();

		int listen(int port);
		XSocket accept();

		int send(const char* buffer, int size);
		int receive(char* buffer, int size_of_buffer, int ms_time = 0);

		int operator! (){ return tcpSock == INVALID_SOCKET; }
		int operator() (){ return tcpSock != INVALID_SOCKET; }

	private:
		int tcp_open();
};

#endif
