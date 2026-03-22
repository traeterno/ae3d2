#ifndef aeSocket
#define aeSocket

#include <nlohmann/json_fwd.hpp>
#include <ae/types.hpp>
using nlohmann::json;

struct pollfd;
struct sockaddr_in;

namespace ae::socket
{

#if defined(__WIN32__) or defined(_WIN32)
typedef unsigned long long Socket;
#else
typedef ae::i32 Socket;
#endif

enum Status { None, Readable, Disconnected };

class Poller
{
public:
	Poller();
	~Poller();
	void setCount(u16 count);
	void set(u16 id, Socket s);
	void reset(u16 id);
	i32 poll(i32 timeout);
	Status get(u16 id);
private:
	pollfd* entries;
	u16 count;
};

void setBlocking(Socket s, bool blocking);
void init();
void shutdown();
ae::i32 getError();

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
	std::string getIP();
	u16 getPort();
private:
	void init();
	friend TcpListener;
	socket::Socket raw;
	std::string ip;
	u16 port;
};

}

#endif