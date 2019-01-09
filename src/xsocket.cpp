#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

#ifdef _WINDOWS_
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock.h>
#else
#include <unistd.h> // gethostname()
#include <limits.h> // HOST_NAME_MAX
#ifdef __HAIKU__
#include <posix/sys/select.h> // fd_set
#endif
#ifndef _WIN32
#include <arpa/inet.h> // ntohl() FIXME: remove
#endif
#endif

#include "xsocket.h"
#define  KDWIN
#include "global.h"

#define XSOCKET_ERROR(str, code) do { \
	if (ErrHUsed) { \
		std::cout<<str<<": "<<code<<"\n"; \
	} else { \
		XSocketLastErrorString = (char *)str; \
		XSocketLastErrorCode = (char *)code; \
		} \
} while(0)

// HOST_NAME_MAX is deprecated; use conservative value
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 255
#endif

static char* XSocketLastErrorString = 0;
static char* XSocketLastErrorCode = 0;
IPaddress XSocketLocalHostADDR;
IPaddress XSocketLocalHostExternADDR;
// HOST_NAME_MAX is POSIX.1-2001
static char XSocketLocalHostName[HOST_NAME_MAX+1];

/*****************************************************************
    Initialization
*****************************************************************/

int XSocketInit(int ErrHUsed)
{
	static int XSocketInitializationOK = 0;

	if(XSocketInitializationOK)
		return 1;

	// SDL_Init() should have already been called before this.
	if (SDLNet_Init() == -1)
	{
		XSOCKET_ERROR("Network init failed", SDLNet_GetError());
		return 0;
	}

	if (gethostname(XSocketLocalHostName, HOST_NAME_MAX) == SOCKET_ERROR)
	{
		XSOCKET_ERROR("gethostname failed", "");
		return 0;
	}
	XSocketLocalHostName[HOST_NAME_MAX] = 0;

	if (SDLNet_ResolveHost(&XSocketLocalHostExternADDR, XSocketLocalHostName, 0) == -1)
	{
		XSOCKET_ERROR("resolvehost failed", SDLNet_GetError());
		return 0;
	}
	XSocketLocalHostADDR = XSocketLocalHostExternADDR;

	// TODO: resolve to external address if hostname resolves to localhost

	XSocketInitializationOK = 1;
	return 1;
}

/*****************************************************************
    Constructor
*****************************************************************/

XSocket::XSocket()
{
	tcpSock = INVALID_SOCKET;
	udpSock = INVALID_SOCKET;
	ErrHUsed = 1;
	addr.host = 0;
	addr.port = 0;
	socketSet = NULL;
}

XSocket::~XSocket()
{
	if(tcpSock != INVALID_SOCKET || udpSock != INVALID_SOCKET)
		close();
}

XSocket::XSocket(XSocket& donor)
{
	tcpSock = donor.tcpSock;
	udpSock = donor.udpSock;
	ErrHUsed = donor.ErrHUsed;
	addr = donor.addr;
	donor.tcpSock = INVALID_SOCKET;
	donor.udpSock = INVALID_SOCKET;
	socketSet = NULL;
}

XSocket& XSocket::operator = (XSocket& donor)
{
	tcpSock = donor.tcpSock;
	udpSock = donor.udpSock;
	ErrHUsed = donor.ErrHUsed;
	addr = donor.addr;
	socketSet = donor.socketSet;
	donor.tcpSock = INVALID_SOCKET;
	donor.udpSock = INVALID_SOCKET;
	donor.socketSet = NULL;
	return *this;
}

/*****************************************************************
    Connect functions
*****************************************************************/

int XSocket::tcp_open()
{
	tcpSock = SDLNet_TCP_Open(&addr);

	if (!tcpSock) {
		XSOCKET_ERROR("TCP socket open failed", SDLNet_GetError());
		return 0;
	}

	socketSet = SDLNet_AllocSocketSet(16);
	SDLNet_TCP_AddSocket(socketSet, tcpSock);

	return 1;
}

/* Open TCP client socket */
int XSocket::open(int IP, int port)
{
	addr.host = htonl(IP);
	addr.port = htons(port);

	return tcp_open();
}

int XSocket::open(char* name,int port)
{
#if _SERVER // FIXME: check actual define name
	if (!name) ErrH.Exit();
#else
	if (!name) ErrH.Abort("NULL inet_addr() argument...");
#endif

	if (SDLNet_ResolveHost(&addr, name, port) == -1)
	{
		XSOCKET_ERROR("TCP socket hostname resolution failed", SDLNet_GetError());
		return 0;
	}

	return tcp_open();
}

/*****************************************************************
    UDP connection function
*****************************************************************/

int XSocket::openUDP(int port)
{
	udpSock = SDLNet_UDP_Open(port);
	if (udpSock == INVALID_SOCKET)
	{
		XSOCKET_ERROR("UDP socket open failed", SDLNet_GetError());
		return 0;
	}

	addr.port = htons(port);
	return 1;
}

// TODO (not needed, just send broadcast UDP?)
int XSocket::open_broadcast(int port)
{
/*	sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	 if(sock == INVALID_SOCKET){
		XSOCKET_ERROR("socket() failed",SDLNet_GetError());
		return 0;
		}

/
	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(port);
	 if(bind(sock, (LPSOCKADDR)&sin, sizeof(sin))){
		XSOCKET_ERROR("bind() failed",SDLNet_GetError());
		return 0;
		}
/
	int optval = 1;
	if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&optval,sizeof(int)) == SOCKET_ERROR){
		XSOCKET_ERROR("setsockopt(SO_BROADCAST) failed",SDLNet_GetError());
		return 0;
		}

	DEFAULT_NON_BLOCKING_OPTION();
	XSocket::port = port;
	return 1;*/
return 0;
}

void XSocket::close()
{
	if (tcpSock)
	{
		SDLNet_TCP_Close(tcpSock);
		tcpSock = INVALID_SOCKET;

	}
	if (udpSock)
	{
		SDLNet_UDP_Close(udpSock);
		udpSock = INVALID_SOCKET;
	}
	if (socketSet) {
		SDLNet_FreeSocketSet(socketSet);
		socketSet = NULL;
	}
	addr.host = 0;
	addr.port = 0;
}

/*****************************************************************
    Listen - Accept functions
*****************************************************************/

int XSocket::listen(int port)
{
	addr.host = INADDR_ANY;
	addr.port = htons(port);

	tcpSock = SDLNet_TCP_Open(&addr);
	if (tcpSock == INVALID_SOCKET)
	{
		XSOCKET_ERROR("TCP listen failed", SDLNet_GetError());
		return 0;
	}

	return 1;
}

XSocket XSocket::accept()
{
	TCPsocket newSock;

	newSock = SDLNet_TCP_Accept(tcpSock);
	if (!newSock)
	{
		XSOCKET_ERROR("TCP accept failed", SDLNet_GetError());
		// TODO: check possible error conditions and results
	}

	XSocket xsock;
	xsock.tcpSock = newSock;
	xsock.ErrHUsed = ErrHUsed;
	xsock.addr.port = addr.port;
	return xsock;
}

// TODO
int XSocket::set_nonblocking_mode(int enable_nonblocking)
{
/*	unsigned long arg = enable_nonblocking;
	if(ioctlsocket(sock,FIONBIO,&arg)){
		XSOCKET_ERROR("ioctlsocket() failed",SDLNet_GetError());
		return 0;
}*/
	return 1;
}

/*****************************************************************
    Send and Receive functions
*****************************************************************/

int XSocket::send(const char* buffer, int size)
{
	int status = 0;
	if (tcpSock)
	{
		status = SDLNet_TCP_Send(tcpSock, buffer, size);
		if (status < size)
		{
			XSOCKET_ERROR("TCP send failed", SDLNet_GetError());
			// TODO: close socket?
			return 0;
		}
		return status;
	}
	else if (udpSock)
	{
		UDPpacket *pkt = SDLNet_AllocPacket(size);
		if (!pkt) // TODO: report oom
			return 0;
// 		pkt->data = buffer; // FIXME: FreePacket deletes this memory?
		memcpy(pkt->data, buffer, size);
		if (!SDLNet_UDP_Send(udpSock, -1, pkt))
		{
			SDLNet_FreePacket(pkt);
			XSOCKET_ERROR("UDP send failed", SDLNet_GetError());
			return 0;
		}
		SDLNet_FreePacket(pkt);
	}
	else
		return 0;

	return 1;
}

int XSocket::receive(char* buffer, int size_of_buffer, int ms_time)
{
	int status = 0;

	if (ms_time == 0) {
		int numready;

		numready = SDLNet_CheckSockets(socketSet, 0);
		if(numready==-1) {
			printf("SDLNet_CheckSockets: %s\n", SDLNet_GetError());
			//most of the time this is a system error, where perror might help you.
			perror("SDLNet_CheckSockets");
		} else if (numready == 0) {
			return 0;
		}

		if(!SDLNet_SocketReady(tcpSock)) {
			return 0;
		}
	}

	if (tcpSock)
	{
		status = SDLNet_TCP_Recv(tcpSock, buffer, size_of_buffer);
		if (status <= 0)
		{
			XSOCKET_ERROR("TCP recv failed", SDLNet_GetError());
			return 0;
		}
		return status;

	}
	else if (udpSock)
	{

		UDPpacket *pkt = SDLNet_AllocPacket(size_of_buffer);
		if (!pkt) // TODO: report oom
			return 0;
// 		pkt->data = buffer; // FIXME: FreePacket deletes this memory?
		if ((status = SDLNet_UDP_Recv(udpSock, pkt)) == -1)
		{
			SDLNet_FreePacket(pkt);
			XSOCKET_ERROR("UDP recv failed", SDLNet_GetError());
			return 0;
		}
		memcpy(buffer, pkt->data, size_of_buffer); //FIXME: use received size, not bufsize
		SDLNet_FreePacket(pkt);
		return status; // 1 - packet was received, 0 - packet was not received.
	}
	else
		return 0;
	
	return 1;
}

/*****************************************************************
    Select functions
*****************************************************************/

// TODO: MODIFY
fd_set XSocketSet;
int XSocket::check_socket(int ms_time)
{
/*	if(sock == INVALID_SOCKET)
		return 0;
	int val;
	FD_ZERO(&XSocketSet);
	FD_SET(sock,&XSocketSet);
	timeval timeout = { ms_time/1000, ms_time % 1000 };
	if((val = select(0, 0, 0, &XSocketSet, &timeout)) == SOCKET_ERROR){
		XSOCKET_ERROR("select() failed",SDLNet_GetError());
		sock = INVALID_SOCKET;
		return 0;
		}
	if(val)
		sock = INVALID_SOCKET;
	return !val;*/
}

// TODO: MODIFY
int XSocket::check_readability(int ms_time)
{
/*	if(sock == INVALID_SOCKET)
		return 0;
	int val;
	FD_ZERO(&XSocketSet);
	FD_SET(sock,&XSocketSet);
	timeval timeout = { ms_time/1000, ms_time % 1000 };
	if((val = select(0, &XSocketSet, 0, 0, &timeout)) == SOCKET_ERROR){
		XSOCKET_ERROR("select() failed",SDLNet_GetError());
		return 0;
		}
	return val;*/
}

// TODO: MODIFY
int XSocket::check_writeability(int ms_time)
{
/*	if(sock == INVALID_SOCKET)
		return 0;
	int val;
	FD_ZERO(&XSocketSet);
	FD_SET(sock,&XSocketSet);
	timeval timeout = { ms_time/1000, ms_time % 1000 };
	if((val = select(0, 0, &XSocketSet, 0, &timeout)) == SOCKET_ERROR){
		XSOCKET_ERROR("select() failed",SDLNet_GetError());
		return 0;
		}
	return val;*/
}

/*****************************************************************
			Send To and Receive From functions
*****************************************************************/
// DO NOT USE?
int XSocket::sendto(const char* buffer, int size, unsigned int IP)
{
/*	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = IP;
	addr.sin_port = htons(port);
	int status = ::sendto(sock, buffer, size, 0, (LPSOCKADDR)&addr, sizeof(SOCKADDR_IN));
	if(status == SOCKET_ERROR){
		int err_code;
		if((err_code = SDLNet_GetError()) == WSAEWOULDBLOCK)
			return 0;
// 		XSOCKET_ERROR("sendto() failed",err_code);
		sock = INVALID_SOCKET;
		return 0;
		}
	return status;*/
}

// DO NOT USE
int XSocket::receivefrom(char* buffer, int size_of_buffer, int ms_time)
{
/*	int status = 0;
	if(check_readability(ms_time)){
		status = recvfrom(sock, buffer, size_of_buffer, 0,0,0);
		if(status == SOCKET_ERROR){
			XSOCKET_ERROR("recvfrom() failed",SDLNet_GetError());
			sock = INVALID_SOCKET;
			return 0;
			}
		}
	return status;*/
}

// WTF?
char* get_name_by_addr(int IP)
{
	IPaddress addr;
	addr.host = htonl(IP);
	return (char *)SDLNet_ResolveIP(&addr); // TODO: erm, check if code that calls this function does anything nasty to the returned buffer...
}
