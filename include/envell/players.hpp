#ifndef aePlayers
#define aePlayers

#include <ae/socket.hpp>
#include <chrono>

namespace envell::players
{

void main(ae::socket::Socket s);

struct Player
{
	ae::net::TcpStream tcp;
};

struct State
{
	ae::socket::Socket main;

	ae::u16 tickRate;
	std::chrono::duration<long long, std::ratio<1, 1000>> tickTime;
	ae::u16 port;

	ae::u8 playerLimit;
	Player* players;
	ae::socket::Poller sockets;
	bool allowNewPlayers;
};

ae::u8 getEmptyID(State* state);

}

#endif