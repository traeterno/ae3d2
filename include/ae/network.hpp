#ifndef aeNetwork
#define aeNetwork

#include <ae/socket.hpp>
#include <thread>

namespace ae
{

class Window;

struct ConnectionAttempt
{
	ae::Window* win; std::string ip; ae::u16 port;
};

class NetworkClient
{
public:
	enum ToServer {};
	enum ToClient {};
	NetworkClient(Window* window);
	~NetworkClient();
	void connect(std::string ip, u16 port);
	void disconnect();
	bool isReady();
private:
	std::thread worker;
	static void startConnection(ae::ConnectionAttempt ca);

	net::TcpStream tcp;
	Window* win;
	bool ready;
};

}

#endif