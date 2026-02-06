#ifndef aeNetwork
#define aeNetwork

#include <ae/types.hpp>
#include <string>

namespace ae::net
{

class TcpSocket;

struct Packet
{
	u8* buf; usize len;
	Packet() = default;
	Packet(std::string msg);
};

class TcpListener
{
public:
	TcpListener(u16 port);
	~TcpListener();
	TcpSocket accept();
private:
	unsigned long long win32;
	int unix;
};

class TcpSocket
{
public:
	TcpSocket();
	TcpSocket(std::string ip, u16 port);
	~TcpSocket();
	void connect(std::string ip, u16 port);
	void disconnect();
	void send(Packet p);
	Packet recv();
private:
	void init();
	friend TcpListener;
	unsigned long long win32;
	int unix;
};

void init();
void shutdown();

}

#endif