#include <cstring>
#include <iostream>
#include <stdio.h>
#include <utility>

#include "xsocket.h"

#define XSOCKET_ERROR(str, code)                      \
	do {                                              \
		if (ErrHUsed) {                               \
			std::cout << str << ": " << code << "\n"; \
		} else {                                      \
			XSocketLastErrorString = (char *)str;     \
			XSocketLastErrorCode = (char *)code;      \
		}                                             \
	} while (0)

static const char *XSocketLastErrorString = nullptr;
static const char *XSocketLastErrorCode = nullptr;
static bool XSocketInitializationOK = false;
std::string XSocketLocalHostAddress;
std::string XSocketLocalHostExternalAddress;

int XSocketInit(int ErrHUsed) {
	if (XSocketInitializationOK)
		return 1;

	// SDL_Init() should have already been called before this.
	if (!NET_Init()) {
		XSOCKET_ERROR("Network init failed", SDL_GetError());
		return 0;
	}

	int addressCount = 0;
	NET_Address **addresses = NET_GetLocalAddresses(&addressCount);
	if (!addresses) {
		XSOCKET_ERROR("Local address lookup failed", SDL_GetError());
		NET_Quit();
		return 0;
	}

	std::string fallbackAddress;
	for (int i = 0; i < addressCount; ++i) {
		const char *addressString = NET_GetAddressString(addresses[i]);
		if (!addressString)
			continue;
		if (fallbackAddress.empty())
			fallbackAddress = addressString;
		if (strcmp(addressString, "127.0.0.1") && strcmp(addressString, "::1")) {
			XSocketLocalHostAddress = addressString;
			break;
		}
	}
	NET_FreeLocalAddresses(addresses);

	if (XSocketLocalHostAddress.empty())
		XSocketLocalHostAddress = fallbackAddress;
	if (XSocketLocalHostAddress.empty()) {
		XSOCKET_ERROR("Local address lookup returned no addresses", "");
		NET_Quit();
		return 0;
	}
	XSocketLocalHostExternalAddress = XSocketLocalHostAddress;

	XSocketInitializationOK = true;
	return 1;
}

void XSocketFinit() {
	if (!XSocketInitializationOK)
		return;
	XSocketLocalHostAddress.clear();
	XSocketLocalHostExternalAddress.clear();
	XSocketInitializationOK = false;
	NET_Quit();
}

XSocket::XSocket() {
	ErrHUsed = 1;
	streamSocket = nullptr;
	serverSocket = nullptr;
	remotePort = 0;
}

XSocket::~XSocket() {
	close();
}

XSocket::XSocket(XSocket &&donor) noexcept {
	streamSocket = donor.streamSocket;
	serverSocket = donor.serverSocket;
	ErrHUsed = donor.ErrHUsed;
	remoteAddress = std::move(donor.remoteAddress);
	remotePort = donor.remotePort;
	donor.streamSocket = nullptr;
	donor.serverSocket = nullptr;
	donor.remotePort = 0;
}

XSocket &XSocket::operator=(XSocket &&donor) noexcept {
	if (this == &donor)
		return *this;
	close();
	streamSocket = donor.streamSocket;
	serverSocket = donor.serverSocket;
	ErrHUsed = donor.ErrHUsed;
	remoteAddress = std::move(donor.remoteAddress);
	remotePort = donor.remotePort;
	donor.streamSocket = nullptr;
	donor.serverSocket = nullptr;
	donor.remotePort = 0;
	return *this;
}

int XSocket::tcp_open(const char *name, int port) {
	close();
	if (!name || port <= 0 || port > 65535) {
		XSOCKET_ERROR("Invalid TCP endpoint", "");
		return 0;
	}
	NET_Address *address = NET_ResolveHostname(name);
	if (!address) {
		XSOCKET_ERROR("TCP socket hostname resolution failed", SDL_GetError());
		return 0;
	}
	if (NET_WaitUntilResolved(address, -1) != NET_SUCCESS) {
		XSOCKET_ERROR("TCP socket hostname resolution failed", SDL_GetError());
		NET_UnrefAddress(address);
		return 0;
	}

	streamSocket = NET_CreateClient(address, static_cast<Uint16>(port), 0);
	if (!streamSocket || NET_WaitUntilConnected(streamSocket, -1) != NET_SUCCESS) {
		XSOCKET_ERROR("TCP socket open failed", SDL_GetError());
		NET_UnrefAddress(address);
		close();
		return 0;
	}
	const char *addressString = NET_GetAddressString(address);
	remoteAddress = addressString ? addressString : name;
	remotePort = port;
	NET_UnrefAddress(address);

	return 1;
}

/* Open TCP client socket */
int XSocket::open(int IP, int port) {
	char address[16];
	snprintf(
		address,
		sizeof(address),
		"%u.%u.%u.%u",
		(IP >> 24) & 0xff,
		(IP >> 16) & 0xff,
		(IP >> 8) & 0xff,
		IP & 0xff
	);
	return tcp_open(address, port);
}

int XSocket::open(const char *name, int port) {
	return tcp_open(name, port);
}

void XSocket::close() {
	if (streamSocket) {
		NET_DestroyStreamSocket(streamSocket);
		streamSocket = nullptr;
	}
	if (serverSocket) {
		NET_DestroyServer(serverSocket);
		serverSocket = nullptr;
	}
	remoteAddress.clear();
	remotePort = 0;
}

int XSocket::listen(int port) {
	close();
	if (port <= 0 || port > 65535) {
		XSOCKET_ERROR("Invalid TCP listen port", "");
		return 0;
	}
	serverSocket = NET_CreateServer(nullptr, static_cast<Uint16>(port), 0);
	if (!serverSocket) {
		XSOCKET_ERROR("TCP listen failed", SDL_GetError());
		return 0;
	}
	remotePort = port;
	return 1;
}

XSocket XSocket::accept() {
	XSocket xsock;
	if (!serverSocket)
		return xsock;
	if (!NET_AcceptClient(serverSocket, &xsock.streamSocket)) {
		XSOCKET_ERROR("TCP accept failed", SDL_GetError());
		return xsock;
	}
	if (!xsock.streamSocket)
		return xsock;
	xsock.ErrHUsed = ErrHUsed;
	xsock.remotePort = remotePort;
	xsock.update_remote_address();

	return xsock;
}

int XSocket::send(const char *buffer, int size) {
	if (!streamSocket || !buffer || size <= 0)
		return 0;

	if (!NET_WriteToStreamSocket(streamSocket, buffer, size)) {
		XSOCKET_ERROR("TCP send failed", SDL_GetError());
		close();
		return 0;
	}
	return size;
}

int XSocket::flush(int ms_time) {
	if (!streamSocket)
		return 0;

	const int pending = NET_WaitUntilStreamSocketDrained(streamSocket, ms_time);
	if (pending < 0) {
		XSOCKET_ERROR("TCP flush failed", SDL_GetError());
		close();
		return 0;
	}
	return pending == 0;
}

int XSocket::receive(char *buffer, int size_of_buffer, int ms_time) {
	if (!streamSocket || !buffer || size_of_buffer <= 0)
		return 0;

	void *socket = streamSocket;
	int available = NET_WaitUntilInputAvailable(&socket, 1, ms_time);
	if (available < 0) {
		close();
		return 0;
	}
	if (available == 0)
		return 0;

	int status = NET_ReadFromStreamSocket(streamSocket, buffer, size_of_buffer);
	if (status < 0) {
		close();
		return 0;
	}
	return status;
}

void XSocket::update_remote_address() {
	if (!streamSocket)
		return;
	NET_Address *address = NET_GetStreamSocketAddress(streamSocket);
	if (!address)
		return;
	const char *addressString = NET_GetAddressString(address);
	if (addressString)
		remoteAddress = addressString;
	NET_UnrefAddress(address);
}
