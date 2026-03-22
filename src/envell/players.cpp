#include <nlohmann/json.hpp>
#include <envell/players.hpp>
#include <chrono>
#include <thread>

void setup(envell::players::State* s, json data);
void handleMessage(envell::players::State* state, ae::socket::Socket s);
void handlePlayer(envell::players::State* state, ae::u8 id, ae::socket::Status status);

void envell::players::main(ae::socket::Socket mainFD)
{
	printf("Starting PlayersMain (PM)\n");

	State state;
	state.main = mainFD;
	state.allowNewPlayers = true;
	state.players = nullptr;

	printf("PM: Waiting for 'setup' message...\n");
	ae::socket::setBlocking(mainFD, true);
	setup:
	{
		auto msg = ae::sync::recv(mainFD);
		if (msg["setup"].empty()) goto setup;
		setup(&state, msg["setup"]);
	}
	ae::socket::setBlocking(mainFD, false);

	ae::net::TcpListener listener;
	listener.bind(state.port);
	state.sockets.set(state.playerLimit, listener.getSocket());
	// state.sockets[state.playersLimit + 1] = UDP
	state.sockets.set(state.playerLimit + 2, mainFD);

	bool running = true;
	while (running)
	{
		int result = state.sockets.poll(-1);
		if (result == -1) printf("Poller error: %i\n", ae::socket::getError());
		for (ae::i32 counter = 0; counter < result; counter++)
		{
			if (state.sockets.get(state.playerLimit + 2) == ae::socket::Readable)
			{
				printf("PM: Message from main thread\n");
				handleMessage(&state, mainFD);
				continue;
			}
			if (state.sockets.get(state.playerLimit) == ae::socket::Readable)
			{
				auto id = getEmptyID(&state);
				if (id == state.playerLimit)
				{
					printf("PM: The party is full. Can't accept more players.\n");
					listener.accept();
					continue;
				}
				auto p = &state.players[id];
				p->tcp = listener.accept();
				state.sockets.set(id, p->tcp.getSocket());
				printf("PM: New player: #%i (%llu)\n", id, p->tcp.getSocket());
				continue;
			}
			for (ae::u8 i = 0; i < state.playerLimit; i++)
			{
				auto status = state.sockets.get(i);
				if (status == ae::socket::None) continue;
				handlePlayer(&state, i, status);
				break;
			}
		}
		std::this_thread::sleep_for(state.tickTime);
	}
}

void handleMessage(envell::players::State* state, ae::socket::Socket s)
{
	auto msg = ae::sync::recv(s);
	if (!msg["setup"].empty())
	{
		setup(state, msg["setup"]);
	}
}

void setup(envell::players::State* s, json data)
{
	printf("PM: Setting up Session...\n");

	s->tickRate = data["tickRate"];
	s->tickTime = std::chrono::milliseconds(1000 / s->tickRate);
	s->port = data["port"];

	if (s->players != nullptr) delete[] s->players;
	s->playerLimit = data["playersCount"];
	s->players = new envell::players::Player[s->playerLimit];
	s->sockets.setCount(s->playerLimit + 3);
}

ae::u8 envell::players::getEmptyID(State* s)
{
	if (!s->allowNewPlayers) return ae::u8max;
	for (ae::u8 i = 0; i < s->playerLimit; i++)
	{
		if (!s->players[i].tcp.getPort()) return i;
	}
	return ae::u8max;
}

void handlePlayer(envell::players::State* state, ae::u8 id, ae::socket::Status status)
{
	auto p = &state->players[id];
	if (status == ae::socket::Disconnected)
	{
		p->tcp.disconnect();
		state->sockets.set(id, 0);
		printf("PM: P%i disconnected.\n", id);
		return;
	}
	auto packet = p->tcp.recv();
	printf("Received %i bytes from P%i\n", packet.len, id);
}