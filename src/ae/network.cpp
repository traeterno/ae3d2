#include <ae/network.hpp>
#include <ae/window.hpp>

ae::NetworkClient::NetworkClient(ae::Window* window)
{
	this->win = window;
	this->worker = std::thread();
	this->ready = false;
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
	// worker = std::thread();
}

void ae::NetworkClient::disconnect()
{
	this->tcp.disconnect();
	this->ready = false;
}

bool ae::NetworkClient::isReady()
{
	return this->ready;
}

void ae::NetworkClient::startConnection(ae::ConnectionAttempt ca)
{
	auto net = ca.win->getNC();

	printf("Trying to connect to %s:%i\n", ca.ip.c_str(), ca.port);
	net->ready = false;
	net->tcp.connect(ca.ip, ca.port);
	if (net->tcp.getPort() == 0) { return; }

	printf("Connected to %s:%i\n", ca.ip.c_str(), ca.port);
	net->ready = true;
}