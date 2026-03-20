#ifndef aeSocket
#define aeSocket

#if defined(__WIN32__) or defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <afunix.h>
#endif

#include <nlohmann/json_fwd.hpp>
#include <ae/types.hpp>
using nlohmann::json;

namespace ae::socket
{

#if defined(__WIN32__) or defined(_WIN32)
typedef unsigned long long Socket;
#else
typedef ae::i32 Socket;
#endif

void setBlocking(Socket s, bool blocking);
void init();
void shutdown();

}

namespace ae::sync
{

socket::Socket* buildPair();
void send(socket::Socket s, json j);
json recv(socket::Socket s);

}

namespace ae::net
{

struct Packet { u8* buf; usize len; };

class TcpStream;

class TcpListener
{
public:
	TcpListener();
	~TcpListener();
	void bind(u16 port);
	TcpStream accept();
	socket::Socket getSocket();
private:
	socket::Socket raw;
};

class TcpStream
{
public:
	TcpStream();
	TcpStream(socket::Socket s, sockaddr_in addr);
	~TcpStream();
	bool connect(std::string ip, u16 port);
	void disconnect();
	void send(Packet p);
	Packet recv();
	socket::Socket getSocket();
private:
	friend TcpListener;
	socket::Socket raw;
	sockaddr_in addr;
};

}

#endif