#include <nlohmann/json.hpp>
#include <envell/players.hpp>
#include <chrono>
#include <thread>

void setup(json data, envell::players::State* s);
void handleMessage(envell::players::State* state, ae::socket::Socket s);

void envell::players::main(ae::socket::Socket mainFD)
{
	printf("Starting PlayersMain (PM)\n");

	State state;
	state.players = nullptr;
	state.sockets = nullptr;

	printf("PM: Waiting for 'setup' message...\n");
	ae::socket::setBlocking(mainFD, true);
	setup:
	{
		auto msg = ae::sync::recv(mainFD);
		if (msg["setup"].empty()) goto setup;
		setup(msg["setup"], &state);
	}
	ae::socket::setBlocking(mainFD, false);

	ae::net::TcpListener listener;
	listener.bind(state.port);

	bool running = true;
	while (running)
	{
		handleMessage(&state, mainFD);
		std::this_thread::sleep_for(state.tickTime);
	}
}

void handleMessage(envell::players::State* state, ae::socket::Socket s)
{
	auto msg = ae::sync::recv(s);
	if (!msg["setup"].empty())
	{
		setup(msg["setup"], state);
	}
}

void setup(json data, envell::players::State* s)
{
	printf("PM: Setting up Session...\n");

	s->tickRate = data["tickRate"];
	s->tickTime = std::chrono::milliseconds(1000 / s->tickRate);
	s->port = data["port"];

	if (s->players != nullptr) delete[] s->players;
	if (s->sockets != nullptr) delete[] s->sockets;
	s->playersLimit = data["playersCount"];
	s->players = new envell::players::Player[s->playersLimit];
	s->sockets = new pollfd[s->playersLimit + 3];
	memset(s->sockets, 0, sizeof(pollfd) * (s->playersLimit + 3));
}