#ifndef _XSOCKET_H
#define _XSOCKET_H

#include <string>

#include <SDL3_net/SDL_net.h>

int XSocketInit(int ErrHUsed = 1);
void XSocketFinit();

// Callback invoked from blocking network waits. It should return true when the
// user asked to abort the current operation. The callback is expected to pump
// input events itself, so it must be cheap.
using XSocketWaitCallback = bool (*)();

void XSocketSetWaitCallback(XSocketWaitCallback callback);
// Polls the wait callback and remembers a cancellation request. Returns true if
// the current wait has been cancelled.
bool XSocketPollCancel();
bool XSocketWasCancelled();
void XSocketResetCancel();

extern std::string XSocketLocalHostAddress;
extern std::string XSocketLocalHostExternalAddress;

class XSocket {
	int ErrHUsed;
	NET_StreamSocket *streamSocket;
	std::string remoteAddress;
	int remotePort;

  public:
	XSocket();
	~XSocket();

	XSocket(const XSocket &) = delete;
	XSocket &operator=(const XSocket &) = delete;
	XSocket(XSocket &&donor) noexcept;
	XSocket &operator=(XSocket &&donor) noexcept;

	int open(int IP, int port);
	int open(const char *name, int port);
	void close();

	int send(const char *buffer, int size);
	int send_if_ready(const char *buffer, int size);
	int flush(int ms_time);
	int receive(char *buffer, int size_of_buffer, int ms_time = 0);

	const std::string &address() const {
		return remoteAddress;
	}
	int port() const {
		return remotePort;
	}
	bool is_open() const {
		return streamSocket != nullptr;
	}

	explicit operator bool() const {
		return is_open();
	}
	bool operator!() const {
		return !static_cast<bool>(*this);
	}

  private:
	int tcp_open(const char *name, int port);
};

#endif
