#ifndef aePlayers
#define aePlayers

#include <ae/sync.hpp>
#include <ae/network.hpp>
#include <chrono>

#if defined(__WIN32__) or defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>

#endif

namespace envell::players
{

void main(ae::sync::Socket s);

struct Player
{
	ae::net::TcpSocket tcp;
};

struct State
{
	ae::u16 tickRate;
	std::chrono::duration<long long, std::ratio<1, 1000>> tickTime;
	ae::u16 port;

	ae::u8 playersLimit;
	Player* players;
	pollfd* sockets;
};

}

#endif