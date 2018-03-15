
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#ifdef _WINDOWS_
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock.h>
#else
#include <unistd.h> // gethostname()
#include <limits.h> // HOST_NAME_MAX
#ifndef _WIN32
#include <arpa/inet.h> // ntohl() FIXME: remove
#endif
#endif

#include "xsocket.h"
#define  KDWIN
//#include "_xtool.h"
#include "../lib/xtool/xtcore.h"


//#define XSOCKET_ABORT(str,code) { if(ErrHUsed) ErrH.Abort(str,XERR_USER,code); else{ XSocketLastErrorString = str; XSocketLastErrorCode = code; }}
#define XSOCKET_ABORT(str,code) { if(ErrHUsed) XCon < str < ", code: " <= code < "                               \n"; else{ XSocketLastErrorString = str; XSocketLastErrorCode = code; }}
#define XSOCKET_WARNING(str,code)
#define	DEFAULT_NON_BLOCKING_OPTION() set_nonblocking_mode(1)

char* XSocketLastErrorString = 0;
int XSocketLastErrorCode = 0;
char XSocketLocalHostName[257];
int XSocketLocalHostADDR;
int XSocketLocalHostExternADDR = 0;
int XSocketInitializationOK = 0;
/*****************************************************************
			Initialization
*****************************************************************/
int XSocketInit(int ErrHUsed)
{
	if(XSocketInitializationOK)
		return 1;
	
	WORD wVersionRequested; 
	WSADATA wsaData; 
	wVersionRequested = MAKEWORD(1, 1); 
													
	 /* инициализация сокетов */
	if(WSAStartup(wVersionRequested, &wsaData) !=  0){
		XSOCKET_ABORT("Winsock failed",WSAGetLastError());
		return 0;
		}

	if(gethostname(XSocketLocalHostName,256) == SOCKET_ERROR){
		XSOCKET_ABORT("Gethostname failed",WSAGetLastError());
		return 0;
		}
	XSocketLocalHostName[256] = 0;

	PHOSTENT phe = gethostbyname(XSocketLocalHostName);
	if(phe == NULL){
		XSOCKET_ABORT("Gethostbyname failed",WSAGetLastError());
		return 0;
		}
	if(phe -> h_length != 4){
		XSOCKET_ABORT("Not a 4-bytes IP address",phe -> h_length);
		return 0;
		}

	in_addr addr;
	addr.s_addr = *((int*)(phe -> h_addr));
	XSocketLocalHostExternADDR =  XSocketLocalHostADDR = addr.s_addr;

	if(phe -> h_addr_list[1]){
		addr.s_addr = *((int*)(phe -> h_addr_list[1]));
		XSocketLocalHostExternADDR = addr.s_addr;
		}

	XSocketInitializationOK = 1;
	return 1;
}
/*****************************************************************
			Constructor
*****************************************************************/
XSocket::XSocket()
{
	sock = INVALID_SOCKET;
	ErrHUsed = 1;
	IP = 0;
	port = 0;
}
XSocket::~XSocket()
{
	if(sock != INVALID_SOCKET)
		close();
}
XSocket::XSocket(XSocket& donor)
{
	sock = donor.sock;
	ErrHUsed = donor.ErrHUsed;
	IP = donor.IP;
	port = donor.port;
	donor.sock = INVALID_SOCKET;
}
XSocket& XSocket::operator = (XSocket& donor)
{
	sock = donor.sock;
	ErrHUsed = donor.ErrHUsed;
	IP = donor.IP;
	port = donor.port;
	donor.sock = INVALID_SOCKET;
	return *this;
}
/*****************************************************************
			Connect functions
*****************************************************************/
int XSocket::open(int IP,int port)
{
	 /* создание сокета с клиентской стороны */
	sock = socket( AF_INET, SOCK_STREAM, 0);
	 if(sock == INVALID_SOCKET){
		XSOCKET_ABORT("socket() failed",WSAGetLastError());
		return 0;
		}

	 /* заполняем структурку dest_sin флажками и IP-адресом и портом сервера */
	SOCKADDR_IN dest_sin;
	dest_sin.sin_family = AF_INET; 
	dest_sin.sin_port = htons(port);    
	dest_sin.sin_addr.s_addr = IP;
			      
	/* теперь готовы: сокет, IP и номер порта сервера - можно конектиться */
	if(connect( sock, (PSOCKADDR) &dest_sin, sizeof(dest_sin)) < 0){
		XSOCKET_ABORT("connect() failed",WSAGetLastError());
		close();
		return 0;
		}
	
	DEFAULT_NON_BLOCKING_OPTION();
	XSocket::IP = IP;
	XSocket::port = port;
	return 1;
}

int XSocket::open(char* name,int port)
{
	unsigned long if_IP;
	if(!name) ErrH.Abort("NULL inet_addr() argument...");
	if((if_IP = inet_addr(name)) != INADDR_NONE)
		return open(if_IP,port);

	 /* создание сокета с клиентской стороны */
	sock = socket( AF_INET, SOCK_STREAM, 0);
	 if(sock == INVALID_SOCKET){
		XSOCKET_ABORT("socket() failed",WSAGetLastError());
		return 0;
		}

	 /* заполняем структурку dest_sin флажками и IP-адресом и портом сервера */
	SOCKADDR_IN dest_sin;
	dest_sin.sin_family = AF_INET; 
	dest_sin.sin_port = htons(port);       
			      
	PHOSTENT phe = gethostbyname(name);
	if(phe == NULL){
		XSOCKET_ABORT("Host unknown",WSAGetLastError());
		return 0;
		}
	memcpy((char FAR *)&(dest_sin.sin_addr), phe -> h_addr,phe -> h_length);

	/* теперь готовы: сокет, IP и номер порта сервера - можно конектиться */
	if(connect( sock, (PSOCKADDR) &dest_sin, sizeof( dest_sin)) < 0){
		XSOCKET_ABORT("connect() failed",WSAGetLastError());
		close();
		return 0;
		}
	
	DEFAULT_NON_BLOCKING_OPTION();
	XSocket::IP = *((int*)(phe -> h_addr));
	XSocket::port = port;
	return 1;
}

/*****************************************************************
			UDP connection function
*****************************************************************/
int XSocket::openUDP(int port)							
{
	sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
	 if(sock == INVALID_SOCKET){
		XSOCKET_ABORT("socket() failed",WSAGetLastError());
		return 0;
		}

	SOCKADDR_IN sin;  
	sin.sin_family = AF_INET; 
	sin.sin_addr.s_addr = 0; 
	sin.sin_port = htons(port); 
	 if(bind(sock, (LPSOCKADDR)&sin, sizeof(sin))){
		XSOCKET_ABORT("bind() failed",WSAGetLastError());
		return 0;
		}
	
	DEFAULT_NON_BLOCKING_OPTION();
	XSocket::port = port;
	return 1;
}
int XSocket::open_broadcast(int port)							
{
	sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
	 if(sock == INVALID_SOCKET){
		XSOCKET_ABORT("socket() failed",WSAGetLastError());
		return 0;
		}

/*
	SOCKADDR_IN sin;  
	sin.sin_family = AF_INET; 
	sin.sin_addr.s_addr = 0; 
	sin.sin_port = htons(port); 
	 if(bind(sock, (LPSOCKADDR)&sin, sizeof(sin))){
		XSOCKET_ABORT("bind() failed",WSAGetLastError());
		return 0;
		}
*/
	int optval = 1;
	if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&optval,sizeof(int)) == SOCKET_ERROR){
		XSOCKET_ABORT("setsockopt(SO_BROADCAST) failed",WSAGetLastError());
		return 0;
		}
	
	DEFAULT_NON_BLOCKING_OPTION();
	XSocket::port = port;
	return 1;
}
/*****************************************************************
			Proxy connection function
*****************************************************************/
char* Socks5ErrorMessages[] = {
"Socks5: succeeded",
"Socks5: general SOCKS server failure",
"Socks5: connection not allowed by ruleset",
"Socks5: Network unreachable",
"Socks5: Host unreachable",
"Socks5: Connection refused",
"Socks5: TTL expired",
"Socks5: Command not supported",
"Socks5: Address type not supported",
"Socks5: to X'FF' unassigned"
};

int XSocket::open_by_socks5(char* name,int port,char* proxy_name,int proxy_port)
{
	if(!open(proxy_name,proxy_port))
		return 0;

	int len;
	XBuffer buf;
	buf < char(5) < char(1) < char(0);
	send(buf,buf.tell());
	while((len = receive(buf,buf.length(),XS_WAITING_TIME)) == SOCKET_ERROR){
		XSOCKET_ABORT("Time out of proxy response 1",0);
		close();
		return 0;
		}

	if(buf[0] != 5 || buf[1] != 0){
		XSOCKET_ABORT("Proxy server not found or incorrect socks version", 0);
		close();
		return 0;
		}
	
	buf.init();
	unsigned long if_IP;
	if(!name) ErrH.Abort("NULL inet_addr() argument (socks5 fnc)...");
	if((if_IP = inet_addr(name)) == INADDR_NONE)
		buf < char(5) < char(1) < char(0) < char(3) < (unsigned char)(strlen(name)) < name < htons(port);       
	else
		buf < char(5) < char(1) < char(0) < char(1) < if_IP < htons(port);       
	send(buf,buf.tell());
	if((len = receive(buf,buf.length(),XS_WAITING_TIME)) == SOCKET_ERROR){
		XSOCKET_ABORT("Time out of proxy response 2", 0);
		close();
		return 0;
		}
	
	if(buf[1] != 0){
		XSOCKET_ABORT(Socks5ErrorMessages[buf[1]],0);
		close();
		return 0;
		}
	
	// XSocket::port = port;
	return 1;
}
   
void XSocket::close()
{
	closesocket(sock);
	sock = INVALID_SOCKET;
	IP = 0;
	port = 0;
}

/*****************************************************************
			Listen - Accept functions
*****************************************************************/
int XSocket::listen(int port)
{
	sock = socket(AF_INET, SOCK_STREAM, 0);
	 if(sock == INVALID_SOCKET){
		XSOCKET_ABORT("socket() failed",WSAGetLastError());
		return 0;
		}

	SOCKADDR_IN sin;  
	sin.sin_family = AF_INET; 
	sin.sin_addr.s_addr = 0; 
	sin.sin_port = htons(port); 
	 if(bind(sock, (LPSOCKADDR)&sin, sizeof(sin))){
		XSOCKET_ABORT("bind() failed",WSAGetLastError());
		return 0;
		}

        if(::listen(sock,5) == SOCKET_ERROR){
		XSOCKET_ABORT("listen() failed",WSAGetLastError());
		return 0;
		}

	DEFAULT_NON_BLOCKING_OPTION();
	XSocket::port = port;
	return 1;
}

XSocket XSocket::accept()
{
        SOCKET new_sock;
	sockaddr addr;
	int addrlen = sizeof(sockaddr);
	if((new_sock = ::accept(sock,  &addr, &addrlen)) == INVALID_SOCKET){
		int err_code;
		if((err_code = WSAGetLastError()) != WSAEWOULDBLOCK){
			XSOCKET_ABORT("accept() failed",err_code);
			sock = INVALID_SOCKET;
			}
		new_sock = INVALID_SOCKET;
		}
	XSocket xsock;
	xsock.sock = new_sock;
	xsock.ErrHUsed = ErrHUsed;
	xsock.port = port;
	return xsock;
}

int XSocket::set_nonblocking_mode(int enable_nonblocking)
{
	unsigned long arg = enable_nonblocking;
	if(ioctlsocket(sock,FIONBIO,&arg)){
		XSOCKET_ABORT("ioctlsocket() failed",WSAGetLastError());
		return 0;
		}
	return 1;
}

/*****************************************************************
			Send and Receive functions
*****************************************************************/
int XSocket::send(const char* buffer, int size)
{
	int status = ::send(sock, buffer, size, 0);
	if(status == SOCKET_ERROR){
		int err_code;
		if((err_code = WSAGetLastError()) == WSAEWOULDBLOCK)
			return 0;
		XSOCKET_ABORT("send() failed",err_code);
		sock = INVALID_SOCKET;
		return 0;
		}
	return status;
}

int XSocket::receive(char* buffer, int size_of_buffer, int ms_time)
{
	int status = 0;
	if(check_readability(ms_time)){
		status = recv(sock, buffer, size_of_buffer, 0);
		if(status == SOCKET_ERROR){
			XSOCKET_WARNING("recv() failed",WSAGetLastError());
			sock = INVALID_SOCKET;
			return 0;
			}
		}
	return status;
}
/*****************************************************************
			Select	functions
*****************************************************************/
fd_set XSocketSet;
int XSocket::check_socket(int ms_time)
{
	if(sock == INVALID_SOCKET)
		return 0;
	int val;
	FD_ZERO(&XSocketSet);
	FD_SET(sock,&XSocketSet);
	timeval timeout = { ms_time/1000, ms_time % 1000 };
	if((val = select(0, 0, 0, &XSocketSet, &timeout)) == SOCKET_ERROR){
		XSOCKET_ABORT("select() failed",WSAGetLastError());
		sock = INVALID_SOCKET;
		return 0;
		}
	if(val)
		sock = INVALID_SOCKET;
	return !val;
}	
int XSocket::check_readability(int ms_time)
{
	if(sock == INVALID_SOCKET)
		return 0;
	int val;
	FD_ZERO(&XSocketSet);
	FD_SET(sock,&XSocketSet);
	timeval timeout = { ms_time/1000, ms_time % 1000 };
	if((val = select(0, &XSocketSet, 0, 0, &timeout)) == SOCKET_ERROR){
		XSOCKET_ABORT("select() failed",WSAGetLastError());
		return 0;
		}
	return val;
}	
int XSocket::check_writeability(int ms_time)
{
	if(sock == INVALID_SOCKET)
		return 0;
	int val;
	FD_ZERO(&XSocketSet);
	FD_SET(sock,&XSocketSet);
	timeval timeout = { ms_time/1000, ms_time % 1000 };
	if((val = select(0, 0, &XSocketSet, 0, &timeout)) == SOCKET_ERROR){
		XSOCKET_ABORT("select() failed",WSAGetLastError());
		return 0;
		}
	return val;
}	
/*****************************************************************
			Send To and Receive From functions
*****************************************************************/
int XSocket::sendto(const char* buffer, int size, unsigned int IP)
{
	SOCKADDR_IN addr;  
	addr.sin_family = AF_INET; 
	addr.sin_addr.s_addr = IP; 
	addr.sin_port = htons(port); 
	int status = ::sendto(sock, buffer, size, 0, (LPSOCKADDR)&addr, sizeof(SOCKADDR_IN));
	if(status == SOCKET_ERROR){
		int err_code;
		if((err_code = WSAGetLastError()) == WSAEWOULDBLOCK)
			return 0;
		XSOCKET_ABORT("sendto() failed",err_code);
		sock = INVALID_SOCKET;
		return 0;
		}
	return status;
}
int XSocket::receivefrom(char* buffer, int size_of_buffer, int ms_time)
{
	int status = 0;
	if(check_readability(ms_time)){
		status = recvfrom(sock, buffer, size_of_buffer, 0,0,0);
		if(status == SOCKET_ERROR){
			XSOCKET_ABORT("recvfrom() failed",WSAGetLastError());
			sock = INVALID_SOCKET;
			return 0;
			}
		}
	return status;
}
char* get_name_by_addr(int IP)
{
	PHOSTENT phe = gethostbyaddr((const char*)&IP,4,AF_INET);
	if(phe && phe -> h_length == 4)
		return phe -> h_name;
	return 0;
}

bool is_power_of_two(int sz) {
	return sz && !(sz & (sz - 1));
}
