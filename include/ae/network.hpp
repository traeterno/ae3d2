#ifndef aeNetwork
#define aeNetwork

#include <ae/socket.hpp>
#include <thread>

namespace ae
{

class Window;

struct ConnectionAttempt
{
	Window* win; std::string ip; ae::u16 port;
};

class NetworkClient
{
public:
	enum class ConnectionStatus
	{
		Idle = 0,
		Disconnected = 1,
		Connected = 2,
		ConnectionFailed = 3,
		Connecting = 4,
		Searching = 5,
		ServerFound = 6,
		ServerNotFound = 7
	};
	enum ToServer {};
	enum ToClient {};
	NetworkClient(Window* window);
	~NetworkClient();
	void connect(std::string ip, u16 port);
	void disconnect();
	void reconnect();
	bool isReady();
	void search();
	void stopSearch();
	socket::IpAddress getServerIP();
	ConnectionStatus getConnectionStatus();
private:
	std::thread worker;
	static void startConnection(ae::ConnectionAttempt ca);
	static void searchServer(Window* win);

	net::TcpStream tcp;
	net::UdpSocket udp;
	Window* win;
	bool ready;
	
	ConnectionStatus cs;

	socket::IpAddress serverIP;
	u16 udpPort;
};

}

#endif