#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WINDOWS_
#	define WIN32_EXTRA_LEAN
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <winsock.h>
#else
#	include <limits.h> // HOST_NAME_MAX
#	include <unistd.h> // gethostname()
#	ifdef __HAIKU__
#		include <posix/sys/select.h> // fd_set
#	endif
#	ifndef _WIN32
#		include <arpa/inet.h> // ntohl() FIXME: remove
#	endif
#endif

#include "xsocket.h"

#define XSOCKET_ERROR(str, code)                      \
	do {                                              \
		if (ErrHUsed) {                               \
			VNG_DEBUG() << str << ": " << code << "\n"; \
		} else {                                      \
			XSocketLastErrorString = (char *)str;     \
			XSocketLastErrorCode = (char *)code;      \
		}                                             \
	} while (0)

// HOST_NAME_MAX is deprecated; use conservative value
#ifndef HOST_NAME_MAX
#	define HOST_NAME_MAX 255
#endif

static char *XSocketLastErrorString = 0;
static char *XSocketLastErrorCode = 0;
IPaddress XSocketLocalHostADDR;
IPaddress XSocketLocalHostExternADDR;
// HOST_NAME_MAX is POSIX.1-2001
static char XSocketLocalHostName[HOST_NAME_MAX + 1];

int XSocketInit(int ErrHUsed) {
	static int XSocketInitializationOK = 0;

	if (XSocketInitializationOK)
		return 1;

	// SDL_Init() should have already been called before this.
	if (SDLNet_Init() == -1) {
		XSOCKET_ERROR("Network init failed", SDLNet_GetError());
		return 0;
	}

	if (gethostname(XSocketLocalHostName, HOST_NAME_MAX) == -1) {
		XSOCKET_ERROR("gethostname failed", "");
		return 0;
	}
	XSocketLocalHostName[HOST_NAME_MAX] = 0;

	if (SDLNet_ResolveHost(&XSocketLocalHostExternADDR, XSocketLocalHostName, 0) == -1) {
		XSOCKET_ERROR("resolvehost failed", SDLNet_GetError());
		return 0;
	}
	XSocketLocalHostADDR = XSocketLocalHostExternADDR;

	// TODO: resolve to external address if hostname resolves to localhost

	XSocketInitializationOK = 1;
	return 1;
}

XSocket::XSocket() {
	tcpSock = NULL;
	ErrHUsed = 1;
	addr.host = 0;
	addr.port = 0;
	socketSet = NULL;
}

XSocket::~XSocket() {
	if (tcpSock != NULL)
		close();
}

XSocket::XSocket(XSocket &donor) {
	tcpSock = donor.tcpSock;
	ErrHUsed = donor.ErrHUsed;
	addr = donor.addr;
	donor.tcpSock = NULL;
	socketSet = NULL;
}

XSocket &XSocket::operator=(XSocket &donor) {
	tcpSock = donor.tcpSock;
	ErrHUsed = donor.ErrHUsed;
	addr = donor.addr;
	socketSet = donor.socketSet;
	donor.tcpSock = NULL;
	donor.socketSet = NULL;
	return *this;
}

int XSocket::tcp_open() {
	tcpSock = SDLNet_TCP_Open(&addr);

	if (!tcpSock) {
		XSOCKET_ERROR("TCP socket open failed", SDLNet_GetError());
		return 0;
	}

	socketSet = SDLNet_AllocSocketSet(64);
	SDLNet_TCP_AddSocket(socketSet, tcpSock);

	return 1;
}

/* Open TCP client socket */
int XSocket::open(int IP, int port) {
	addr.host = htonl(IP);
	addr.port = htons(port);

	return tcp_open();
}

int XSocket::open(char *name, int port) {
	if (!name)
		ErrH.Abort("NULL inet_addr() argument...");

	if (SDLNet_ResolveHost(&addr, name, port) == -1) {
		XSOCKET_ERROR("TCP socket hostname resolution failed", SDLNet_GetError());
		return 0;
	}

	return tcp_open();
}

void XSocket::close() {
	if (tcpSock) {
		SDLNet_TCP_Close(tcpSock);
		tcpSock = NULL;
	}
	if (socketSet) {
		SDLNet_FreeSocketSet(socketSet);
		socketSet = NULL;
	}
	addr.host = 0;
	addr.port = 0;
}

int XSocket::listen(int port) {
	addr.host = INADDR_ANY;
	addr.port = htons(port);

	tcpSock = SDLNet_TCP_Open(&addr);
	if (tcpSock == NULL) {
		XSOCKET_ERROR("TCP listen failed", SDLNet_GetError());
		return 0;
	}

	return 1;
}

XSocket XSocket::accept() {
	XSocket xsock;
	TCPsocket newSock;

	newSock = SDLNet_TCP_Accept(tcpSock);
	if (!newSock) // TODO: Should emit error here
		return xsock;

	xsock.tcpSock = newSock;
	xsock.ErrHUsed = ErrHUsed;
	xsock.addr.port = addr.port;
	xsock.socketSet = SDLNet_AllocSocketSet(16);
	SDLNet_TCP_AddSocket(xsock.socketSet, newSock);

	return xsock;
}

int XSocket::send(const char *buffer, int size) {
	if (!tcpSock)
		return 0;

	int status = SDLNet_TCP_Send(tcpSock, buffer, size);
	if (status < size) {
		XSOCKET_ERROR("TCP send failed", SDLNet_GetError()); // TODO: close socket?
		return 0;
	}
	return status;
}

int XSocket::receive(char *buffer, int size_of_buffer, int ms_time) {
	if (ms_time == 0) {
		int n;

		n = SDLNet_CheckSockets(socketSet, 0);
		if (n == -1) {
			printf("SDLNet_CheckSockets: %s\n", SDLNet_GetError());
			// most of the time this is a system error, where perror might help you.
			perror("SDLNet_CheckSockets");
		} else if (n == 0) {
			return 0;
		}

		if (!SDLNet_SocketReady(tcpSock)) {
			return 0;
		}
	}

	if (!tcpSock)
		return 0;

	int status = SDLNet_TCP_Recv(tcpSock, buffer, size_of_buffer);
	if (status <= 0) {
		close();
		return 0;
	}
	return status;
}
