#if defined(__WIN32__) or defined(_WIN32)

#include <ae/network.hpp>
#include <stdio.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// #include <windows.h>
#include <winsock2.h>
// #include <ws2tcpip.h>
// #include <iphlpapi.h>

using namespace ae::net;

Packet::Packet(std::string msg)
{
	this->len = msg.length();
	this->buf = new u8[this->len];
	memcpy(this->buf, msg.c_str(), this->len);
}

TcpListener::TcpListener(u16 port)
{
	printf("Creating WinSocket\n");

	SOCKADDR_IN ip;
	ip.sin_family = AF_INET;
	ip.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	ip.sin_port = htons(port);

	this->win32 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int bindRes = bind(win32, (SOCKADDR*)&ip, sizeof(ip));
	if (bindRes == SOCKET_ERROR)
	{
		printf("TcpListener (%llu): BIND %i\n",
			this->win32, WSAGetLastError()
		);
		closesocket(this->win32);
		this->win32 = 0;
		return;
	}
	int listenRes = listen(this->win32, 8);
	if (listenRes == SOCKET_ERROR)
	{
		printf("TcpListener (%llu): LISTEN %i\n",
			this->win32, WSAGetLastError()
		);
		closesocket(this->win32);
		this->win32 = 0;
		return;
	}
	printf("TcpListener (%llu) is open at port: %i\n", this->win32, port);
}

TcpListener::~TcpListener()
{
	printf("Destroying WinSocket\n");
}

TcpSocket TcpListener::accept()
{
	TcpSocket s;
	SOCKADDR_IN addr;
	printf("Waiting for client...\n");
	s.win32 = ::accept(this->win32, (SOCKADDR*)&addr, NULL);
	printf("Accepted client (%llu)\n", s.win32);
	return s;
}

TcpSocket::TcpSocket() { this->init(); }

TcpSocket::TcpSocket(std::string ip, u16 port)
{
	this->init();
	this->connect(ip, port);
}

TcpSocket::~TcpSocket()
{
	if (this->win32)
	{
		closesocket(this->win32);
		this->win32 = 0;
	}
}

void TcpSocket::connect(std::string ip, u16 port)
{
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

	printf("Trying to connect to (%s:%i)\n", ip.c_str(), port);

	int result;
	if ((result = ::connect(this->win32, (SOCKADDR*)&addr, sizeof(addr))))
	{
		printf("Failed to connect to (%s:%i): %i\n",
			ip.c_str(), port, WSAGetLastError()
		);
	}
	printf("Connected to (%s:%i)\n", ip.c_str(), port);
}

void TcpSocket::disconnect()
{
	::shutdown(this->win32, SD_BOTH);
}

void TcpSocket::send(Packet p)
{
	int size = ::send(
		this->win32, reinterpret_cast<const i8*>(p.buf), p.len, 0
	);
	printf("Sent %i bytes\n", size);
}

Packet TcpSocket::recv()
{
	Packet out;
	char buf[1024];
	int size = ::recv(this->win32, buf, 1024, 0);
	out.buf = reinterpret_cast<u8*>(buf);
	out.len = size;
	return out;
}

void TcpSocket::init()
{
	this->win32 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->win32 == INVALID_SOCKET)
	{
		printf("Failed to create TcpSocket: %i\n", WSAGetLastError());
		this->win32 = 0;
	}
}

void ae::net::init()
{
	printf("Initializing WinSockets\n");

	WSADATA data;
	int result = WSAStartup(MAKEWORD(2, 2), &data);
	if (result != 0)
	{
		printf("Failed to initialize WinSockets: %i\n", result);
	}

	printf("Finished initializing WinSockets\n");
}

void ae::net::shutdown()
{
	printf("Destroying WinSockets\n");
	WSACleanup();
}

#endif