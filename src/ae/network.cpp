#include <ae/network.hpp>
#include <ae/window.hpp>

ae::NetworkClient::NetworkClient(ae::Window* window)
{
	this->win = window;
	this->worker = std::thread();
	this->ready = false;
	this->serverIP = { "", 0 };
	this->tcp = net::TcpStream();
	this->udp = net::UdpSocket();
	this->udpPort = 0;
	this->cs = ConnectionStatus::Idle;
}

ae::NetworkClient::~NetworkClient()
{
	this->tcp.disconnect();
	if (this->worker.joinable()) this->worker.join();
}

void ae::NetworkClient::connect(std::string ip, u16 port)
{
	if (worker.joinable()) { worker.join(); }
	worker = std::thread(startConnection, ConnectionAttempt {
		.win = this->win, .ip = ip, .port = port
	});
}

void ae::NetworkClient::disconnect()
{
	this->tcp.disconnect();
	this->cs = ConnectionStatus::Idle;
	this->serverIP = { "", 0 };
	this->ready = false;
}

void ae::NetworkClient::reconnect()
{
	if (this->serverIP.second == 0) return;
	this->connect(this->serverIP.first, this->serverIP.second);
}

void ae::NetworkClient::search()
{
	if (worker.joinable()) { worker.join(); }
	worker = std::thread(searchServer, this->win);
}

void ae::NetworkClient::stopSearch()
{
	if (!worker.joinable()) return;
	this->cs = ConnectionStatus::Idle;
	this->udp.unbind();
	worker.join();
}

bool ae::NetworkClient::isReady() { return this->ready; }
ae::socket::IpAddress ae::NetworkClient::getServerIP() { return this->serverIP; }
ae::NetworkClient::ConnectionStatus ae::NetworkClient::getConnectionStatus()
{
	return this->cs;
}

void ae::NetworkClient::startConnection(ae::ConnectionAttempt ca)
{
	auto net = ca.win->getNC();

	net->ready = false;
	net->cs = ConnectionStatus::Connecting;
	net->tcp.connect(ca.ip, ca.port);
	if (net->tcp.getPort() == 0)
	{
		net->cs = ConnectionStatus::ConnectionFailed;
		return;
	}

	net->cs = ConnectionStatus::Connected;
	net->ready = true;
	net->serverIP = { ca.ip, ca.port };
}

void ae::NetworkClient::searchServer(Window* win)
{
	auto net = win->getNC();

	net->udp.bind(0);
	net->udp.setBroadcast(true);
	net->udp.setTimeout(2, 2);
	auto buf = new u8[2];
	buf[0] = ((u16)26225 >> 8); buf[1] = (26225 & 255);

	net->serverIP = { "0.0.0.0", 0 };
	net->cs = ConnectionStatus::Searching;
	for (u8 i = 0; i < 5; i++)
	{
		if (net->udp.getPort() == 0) break;
		net->udp.send({ "255.255.255.255", 26225 }, { buf, 2 });
		auto [ip, packet] = net->udp.recv();
		if (packet.len == 0) { net->serverIP = { "0.0.0.0", i + 1 }; continue; }

		u16 port = ((u16)packet.buf[0] << 8) + packet.buf[1];
		delete[] buf;
		delete[] packet.buf;
		net->serverIP = { ip.first, port };
		net->udpPort = port;
		net->cs = ConnectionStatus::ServerFound;
		return;
	}
	delete[] buf;
	net->serverIP = { "", 0 };
	net->cs = ConnectionStatus::ServerNotFound;
}