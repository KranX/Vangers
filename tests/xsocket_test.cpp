#include "xsocket.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <utility>
#include <vector>

#include <SDL3/SDL.h>

namespace {

bool check(bool condition, const char *message) {
	if (!condition)
		std::cerr << "FAIL: " << message << '\n';
	return condition;
}

int listen_on_available_port(XSocket &server) {
	for (int port = 32197; port < 32297; ++port) {
		if (server.listen(port))
			return port;
	}
	return 0;
}

XSocket wait_for_client(XSocket &server) {
	const Uint64 deadline = SDL_GetTicks() + 2000;
	do {
		XSocket client = server.accept();
		if (client)
			return client;
		SDL_Delay(1);
	} while (SDL_GetTicks() < deadline);
	return {};
}

int receive_exact(XSocket &socket, char *buffer, int size) {
	int received = 0;
	const Uint64 deadline = SDL_GetTicks() + 2000;
	while (received < size && SDL_GetTicks() < deadline) {
		const int amount = socket.receive(buffer + received, size - received, 50);
		if (amount > 0)
			received += amount;
		else if (!socket)
			break;
	}
	return received;
}

bool run_send_backpressure_test() {
	XSocket listener;
	const int port = listen_on_available_port(listener);
	if (!check(port != 0, "could not allocate a backpressure test port"))
		return false;

	XSocket client;
	if (!check(client.open("127.0.0.1", port), "backpressure client connection failed"))
		return false;
	XSocket accepted = wait_for_client(listener);
	if (!check(accepted.is_open(), "backpressure peer was not accepted"))
		return false;

	constexpr int chunkSize = 1024 * 1024;
	constexpr int maxChunks = 64;
	std::vector<char> payload(chunkSize, 'Q');
	int acceptedChunks = 0;
	for (; acceptedChunks < maxChunks; ++acceptedChunks) {
		const int sent = client.send_if_ready(payload.data(), payload.size());
		if (sent == 0)
			break;
		if (!check(sent == chunkSize, "ready send returned a partial chunk"))
			return false;
	}
	if (!check(acceptedChunks < maxChunks, "send path did not apply backpressure"))
		return false;
	if (!check(client.is_open(), "backpressure closed a healthy socket"))
		return false;

	const char blockedMarker = 'X';
	for (int i = 0; i < 4; ++i) {
		if (!check(
				client.send_if_ready(&blockedMarker, 1) == 0,
				"pending output accepted another write"
			))
			return false;
	}

	std::vector<char> receiveBuffer(64 * 1024);
	const std::size_t expectedBytes = static_cast<std::size_t>(acceptedChunks) * chunkSize;
	std::size_t receivedBytes = 0;
	const Uint64 deadline = SDL_GetTicks() + 10000;
	while (receivedBytes < expectedBytes && SDL_GetTicks() < deadline) {
		client.flush(0);
		const int amount = accepted.receive(
			receiveBuffer.data(),
			static_cast<int>(std::min(receiveBuffer.size(), expectedBytes - receivedBytes)),
			20
		);
		if (amount <= 0) {
			if (!accepted || !client)
				break;
			continue;
		}
		if (!check(
				std::all_of(
					receiveBuffer.begin(),
					receiveBuffer.begin() + amount,
					[](char value) { return value == 'Q'; }
				),
				"blocked writes leaked into the stream"
			))
			return false;
		receivedBytes += amount;
	}
	if (!check(receivedBytes == expectedBytes, "queued payload did not drain"))
		return false;
	if (!check(client.flush(2000), "backpressure payload remained pending after peer drain"))
		return false;

	const char recoveryMarker = 'R';
	if (!check(
			client.send_if_ready(&recoveryMarker, 1) == 1,
			"send path did not recover after pending output drained"
		))
		return false;
	if (!check(client.flush(2000), "recovery marker did not drain"))
		return false;
	char receivedMarker = 0;
	if (!check(
			receive_exact(accepted, &receivedMarker, 1) == 1 && receivedMarker == recoveryMarker,
			"recovery marker was missing or reordered"
		))
		return false;

	client.close();
	accepted.close();
	listener.close();
	return true;
}

bool run_loopback_test() {
	XSocket listener;
	const int port = listen_on_available_port(listener);
	if (!check(port != 0, "could not allocate a loopback test port"))
		return false;
	if (!check(listener.is_open(), "listener should report an open server socket"))
		return false;
	if (!check(!listener.accept(), "accept without a pending client should be nonblocking"))
		return false;

	XSocket client;
	if (!check(client.open("127.0.0.1", port), "loopback client connection failed"))
		return false;
	XSocket accepted = wait_for_client(listener);
	if (!check(accepted.is_open(), "server did not accept the loopback client"))
		return false;
	if (!check(!accepted.address().empty(), "accepted socket has no peer address"))
		return false;

	char buffer[32] = {};
	const Uint64 timeoutStart = SDL_GetTicks();
	if (!check(accepted.receive(buffer, sizeof(buffer), 30) == 0, "idle receive returned data"))
		return false;
	const Uint64 timeoutElapsed = SDL_GetTicks() - timeoutStart;
	if (!check(timeoutElapsed >= 15 && timeoutElapsed < 1000, "receive timeout was not honored"))
		return false;
	if (!check(accepted.is_open(), "receive timeout closed a healthy socket"))
		return false;
	if (!check(
			accepted.receive(buffer, sizeof(buffer), 0) == 0, "nonblocking receive returned data"
		))
		return false;

	const char first[] = "ordered-";
	const char second[] = "payload";
	if (!check(client.send(first, sizeof(first) - 1) == sizeof(first) - 1, "first send failed"))
		return false;
	if (!check(client.send(second, sizeof(second) - 1) == sizeof(second) - 1, "second send failed"))
		return false;
	const int payloadSize = sizeof(first) + sizeof(second) - 2;
	if (!check(
			receive_exact(accepted, buffer, payloadSize) == payloadSize,
			"ordered payload was incomplete"
		))
		return false;
	if (!check(
			!std::memcmp(buffer, "ordered-payload", payloadSize), "stream payload order changed"
		))
		return false;

	const char partialPayload[] = "0123456789";
	if (!check(
			client.send(partialPayload, sizeof(partialPayload) - 1) == sizeof(partialPayload) - 1,
			"partial-read payload send failed"
		))
		return false;
	const int firstRead = accepted.receive(buffer, 4, 1000);
	if (!check(firstRead > 0 && firstRead <= 4, "bounded receive ignored its destination size"))
		return false;
	const int remaining = static_cast<int>(sizeof(partialPayload) - 1) - firstRead;
	if (!check(
			receive_exact(accepted, buffer + firstRead, remaining) == remaining,
			"partial-read payload was not preserved"
		))
		return false;
	if (!check(
			!std::memcmp(buffer, partialPayload, sizeof(partialPayload) - 1),
			"partial reads changed stream data"
		))
		return false;

	XSocket moved(std::move(accepted));
	if (!check(!accepted && moved, "move construction did not transfer socket ownership"))
		return false;
	XSocket assigned;
	assigned = std::move(moved);
	if (!check(!moved && assigned, "move assignment did not transfer socket ownership"))
		return false;

	const char finalPayload[] = "graceful-close";
	if (!check(
			client.send(finalPayload, sizeof(finalPayload) - 1) == sizeof(finalPayload) - 1,
			"final payload send failed"
		))
		return false;
	if (!check(client.flush(2000), "final payload did not drain before close"))
		return false;
	client.close();
	if (!check(
			receive_exact(assigned, buffer, sizeof(finalPayload) - 1) == sizeof(finalPayload) - 1,
			"drained payload was abandoned during close"
		))
		return false;
	if (!check(
			!std::memcmp(buffer, finalPayload, sizeof(finalPayload) - 1),
			"drained payload changed before close"
		))
		return false;
	const Uint64 disconnectDeadline = SDL_GetTicks() + 2000;
	while (assigned && SDL_GetTicks() < disconnectDeadline)
		assigned.receive(buffer, sizeof(buffer), 50);
	if (!check(!assigned, "peer disconnect did not close the stream socket"))
		return false;

	XSocket integerAddressClient;
	if (!check(
			integerAddressClient.open(0x7f000001, port),
			"host-order integer loopback address did not resolve to 127.0.0.1"
		))
		return false;
	XSocket integerAddressAccepted = wait_for_client(listener);
	if (!check(
			static_cast<bool>(integerAddressAccepted),
			"server did not accept the integer-address client"
		))
		return false;
	const char integerAddressPayload[] = "integer-address";
	if (!check(
			integerAddressClient.send(integerAddressPayload, sizeof(integerAddressPayload) - 1) ==
				sizeof(integerAddressPayload) - 1,
			"integer-address client send failed"
		))
		return false;
	if (!check(
			receive_exact(integerAddressAccepted, buffer, sizeof(integerAddressPayload) - 1) ==
				sizeof(integerAddressPayload) - 1,
			"integer-address client payload was incomplete"
		))
		return false;
	if (!check(
			!std::memcmp(buffer, integerAddressPayload, sizeof(integerAddressPayload) - 1),
			"integer-address client payload changed"
		))
		return false;

	integerAddressClient.close();
	integerAddressAccepted.close();
	listener.close();
	XSocket refusedClient;
	if (!check(!refusedClient.open("127.0.0.1", port), "connection to a closed listener succeeded"))
		return false;
	if (!check(!refusedClient.is_open(), "failed connection left a stream socket open"))
		return false;

	return true;
}

} // namespace

int main() {
	if (!check(SDL_Init(0), "SDL initialization failed"))
		return 1;
	if (!check(XSocketInit(0), "SDL3_net initialization failed"))
		return 1;
	if (!check(!XSocketLocalHostAddress.empty(), "local address discovery returned no address"))
		return 1;

	bool success = false;
	{
		XSocket invalid;
		success = check(!invalid.listen(0), "invalid listen port was accepted") &&
				  check(!invalid.open("127.0.0.1", 0), "invalid client port was accepted") &&
				  check(!invalid.flush(0), "closed socket reported a successful flush") &&
				  run_loopback_test() && run_send_backpressure_test();
	}
	XSocketFinit();

	if (!check(XSocketInit(0), "SDL3_net failed to reinitialize after shutdown"))
		success = false;
	XSocketFinit();
	SDL_Quit();
	return success ? 0 : 1;
}
