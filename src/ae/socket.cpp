#include <ae/global.hpp>
#include <ae/socket.hpp>
#include <filesystem>
#include <thread>
#include <nlohmann/json.hpp>

#if defined(__WIN32__) or defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <afunix.h>
#include <ws2tcpip.h>
#endif

#if defined(__WIN32__) or defined(_WIN32)
ae::i32 ae::socket::getError() { return WSAGetLastError(); }
#else
// TODO Linux implementation
#endif

ae::socket::Poller::Poller()
{
	this->entries = nullptr;
	this->count = 0;
}

ae::socket::Poller::~Poller()
{
	if (this->entries != nullptr)
	{
		delete[] this->entries;
	}
}

void ae::socket::Poller::setCount(u16 count)
{
	if (count == 0)
	{
		delete[] this->entries;
		this->count = 0;
		return;
	}
	auto buf = new pollfd[count];
	delete[] this->entries;
	this->entries = buf;
	for (u16 i = 0; i < count; i++)
	{
		this->entries[i] = { .fd = 0, .events = 0, .revents = 0 };
	}
	this->count = count;
}

void ae::socket::Poller::set(u16 id, Socket s)
{
	if (id > this->count)
	{
		printf("Poller: ID #%i is greater than size %i\n",
			id, this->count
		);
		return;
	}
	this->entries[id] = { .fd = s, .events = POLLIN, .revents = 0 };
}

void ae::socket::Poller::reset(u16 id)
{
	if (id > this->count)
	{
		printf("Poller: ID #%i is greater than size %i\n",
			id, this->count
		);
		return;
	}
	this->entries[id] = { .fd = 0, .events = 0, .revents = 0 };
}

ae::i32 ae::socket::Poller::poll(i32 timeout)
{
	i32 result = 0;
	#if defined(__WIN32__) or defined(_WIN32)
	result = WSAPoll(this->entries, this->count, timeout);
	#else
	// TODO Linux implementation
	#endif
	return result;
}

ae::socket::Status ae::socket::Poller::get(u16 id)
{
	if (id > this->count)
	{
		printf("Poller: ID #%i is greater than size %i\n",
			id, this->count
		);
		return Status::None;
	}
	if (this->entries[id].fd == 0) return Status::None;

	auto status = this->entries[id].revents;
	this->entries[id].revents = 0;
	if (status == 0) return Status::None;
	if (status & POLLRDNORM) return Status::Readable;
	if (status & POLLHUP) return Status::Disconnected;
	printf("Unknown poll status: %llu %i\n", this->entries[id].fd, status);
	return Status::None;
}

void ae::socket::setBlocking(Socket s, bool blocking)
{
	#if defined(__WIN32__) or defined(_WIN32)
	unsigned long mode = blocking ? 0 : 1;
	ioctlsocket(s, FIONBIO, &mode);
	#else
	// TODO Linux implementation
	#endif
}

void ae::socket::init()
{
	#if defined(__WIN32__) or defined(_WIN32)
	WSADATA data;
	int result = WSAStartup(MAKEWORD(2, 2), &data);
	if (result != 0)
	{
		printf("Failed to initialize WinSockets: %i\n", result);
		return;
	}
	#endif
	srand(time(0));
	printf("Initialized WinSockets\n");
}

void ae::socket::shutdown()
{
	#if defined(__WIN32__) or defined(_WIN32)
	WSACleanup();
	#endif
	printf("Destroyed WinSockets\n");
}

ae::socket::Socket* ae::sync::buildPair()
{
	std::string temp = std::filesystem::temp_directory_path().string();
	temp += "ae3d";
	if (!std::filesystem::exists(temp))
	{
		std::filesystem::create_directory(temp);
	}
	auto path = ae::str::format("%s/aeSock%i",
		temp.c_str(), rand() % 26225
	);
	::unlink(path.c_str());
	sockaddr_un addr;
	memset(&addr, 0, sizeof(sockaddr_un));
	addr.sun_family = AF_UNIX;
	strcpy_s(addr.sun_path, path.c_str());

	auto server = ::socket(AF_UNIX, SOCK_STREAM, 0);
	if (server == INVALID_SOCKET)
	{
		printf("Failed to create sync server\n");
		return nullptr;
	}
	if (bind(server, (sockaddr*)&addr, sizeof(sockaddr_un)) == SOCKET_ERROR)
	{
		printf("Failed to bind sync server\n");
		closesocket(server);
		return nullptr;
	}
	if (listen(server, 0) != 0)
	{
		printf("Failed to listene on sync server\n");
		closesocket(server);
		return nullptr;
	}
	auto pair = new socket::Socket[2];
	pair[0] = 0; pair[1] = 0;
	std::thread t([&pair, &addr]()
	{
		pair[0] = ::socket(AF_UNIX, SOCK_STREAM, 0);
		if (pair[0] < 0)
		{
			printf("Failed to create sync socket\n");
			pair[0] = -1;
			return;
		}
		if (connect(pair[0], (sockaddr*)&addr, sizeof(sockaddr_un)) == SOCKET_ERROR)
		{
			printf("Failed to connect to sync server\n");
			pair[0] = -1;
			return;
		}
	});
	pair[1] = accept(server, NULL, NULL);
	t.join();
	if (pair[0] < 0 || pair[1] < 0)
	{
		delete[] pair;
		return nullptr;
	}
	socket::setBlocking(pair[0], false);
	socket::setBlocking(pair[1], true);
	closesocket(server);
	return pair;
}

void ae::sync::send(socket::Socket s, json data)
{
	std::string out = data.dump();
	::send(s, out.c_str(), out.length(), 0);
}

json ae::sync::recv(socket::Socket s)
{
	auto buf = new char[16*1024];
	auto len = ::recv(s, buf, 16*1024, 0);
	if (len == -1) { delete[] buf; return json(); }
	buf[len] = 0;
	auto v = json::parse(buf);
	delete[] buf;
	return v;
}

ae::net::TcpListener::TcpListener()
{
	this->port = 0;
	this->raw = ::socket(AF_INET, SOCK_STREAM, 0);
	if (this->raw < 0)
	{
		printf("Failed to create TcpListener socket\n");
		this->raw = 0;
		return;
	}
}

ae::net::TcpListener::~TcpListener()
{
	if (this->raw != 0)
	{
		closesocket(this->raw);
	}
}

void ae::net::TcpListener::bind(u16 port)
{
	if (this->raw == 0) { *this = TcpListener(); }

	sockaddr_in ip;
	i32 size = sizeof(sockaddr_in);
	memset(&ip, 0, size);
	ip.sin_family = AF_INET;
	ip.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	ip.sin_port = htons(port);
	if (::bind(this->raw, (sockaddr*)&ip, size) == SOCKET_ERROR)
	{
		printf("Failed to bind listener\n");
		closesocket(this->raw);
		this->raw = 0;
		return;
	}
	if (::listen(this->raw, 0) == SOCKET_ERROR)
	{
		printf("Failed to start listener\n");
		closesocket(this->raw);
		this->raw = 0;
		return;
	}

	getsockname(this->raw, (sockaddr*)&ip, &size);
	this->port = ntohs(ip.sin_port);
	printf("Started listener on port %i\n", this->port);
}

ae::u16 ae::net::TcpListener::getPort()
{
	sockaddr_in addr;
	i32 size = sizeof(sockaddr_in);
	getsockname(this->raw, (sockaddr*)&addr, &size);
	this->port = ntohs(addr.sin_port);
	return this->port;
}

ae::net::TcpStream ae::net::TcpListener::accept()
{
	sockaddr_in addr;
	i32 size = sizeof(sockaddr_in);
	memset(&addr, 0, size);
	socket::Socket raw = ::accept(this->raw, (sockaddr*)&addr, &size);
	std::string ip = inet_ntoa(addr.sin_addr);
	printf("Accepted client %s:%i\n", ip.c_str(), ntohs(addr.sin_port));
	return TcpStream(raw, addr);
}

ae::socket::Socket ae::net::TcpListener::getSocket() { return this->raw; }

ae::net::TcpStream::TcpStream()
{
	this->raw = 0;
	this->ip = "";
	this->port = 0;
}

ae::net::TcpStream::TcpStream(socket::Socket s, sockaddr_in addr)
{
	this->raw = s;
	this->ip = inet_ntoa(addr.sin_addr);
	this->port = ntohs(addr.sin_port);
}

ae::net::TcpStream::~TcpStream()
{
	if (this->raw != 0 && this->port == 0)
	{
		closesocket(this->raw);
	}
}

void ae::net::TcpStream::connect(std::string ip, u16 port)
{
	if (this->port != 0) { this->disconnect(); }
	if (this->raw == 0) { this->init(); }
	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	
	if (::connect(this->raw, (sockaddr*)&addr, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		printf("Failed to connect to %s:%i: %i\n", ip.c_str(), port, socket::getError());
		return;
	}

	this->ip = inet_ntoa(addr.sin_addr);
	this->port = ntohs(addr.sin_port);

	return;
}

void ae::net::TcpStream::disconnect()
{
	if (this->port == 0) return;
	closesocket(this->raw);
	this->raw = 0;
	this->ip.clear();
	this->port = 0;
}

void ae::net::TcpStream::send(Packet p)
{
	::send(this->raw, (i8*)p.buf, p.len, 0);
}

ae::net::Packet ae::net::TcpStream::recv()
{
	Packet out;
	auto buf = new char[8*1024];
	auto size = ::recv(this->raw, buf, 8*1024, 0);
	out.len = size;
	out.buf = new u8[size];
	memcpy(out.buf, buf, size);
	delete[] buf;
	return out;
}

ae::socket::Socket ae::net::TcpStream::getSocket() { return this->raw; }
std::string ae::net::TcpStream::getIP() { return this->ip; }
ae::u16 ae::net::TcpStream::getPort() { return this->port; }

void ae::net::TcpStream::init()
{
	if (this->raw != 0) { closesocket(this->raw); }
	this->raw = ::socket(AF_INET, SOCK_STREAM, 0);
	if (this->raw == INVALID_SOCKET)
	{
		printf("Failed to create stream: %i\n",
			socket::getError()
		);
		this->raw = 0;
	}
	this->ip.clear();
	this->port = 0;
}

ae::net::UdpSocket::UdpSocket()
{
	this->raw = 0;
	this->port = 0;
}

ae::net::UdpSocket::~UdpSocket()
{
	if (this->port != 0) return;
	closesocket(this->raw);
}

void ae::net::UdpSocket::bind(u16 port)
{
	if (this->port != 0) { this->unbind(); }
	if (this->raw == 0) { this->init(); }
	
	sockaddr_in addr;
	i32 size = sizeof(sockaddr_in);
	memset(&addr, 0, size);
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if (::bind(this->raw, (sockaddr*)&addr, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		printf("Failed to bind UDP to port %i: %i\n", port, socket::getError());
		closesocket(this->raw);
		return;
	}

	getsockname(this->raw, (sockaddr*)&addr, &size);
	
	this->port = ntohs(addr.sin_port);
	printf("Bound UDP to port %i (expected %i)\n", this->port, port);
}

void ae::net::UdpSocket::unbind()
{
	closesocket(this->raw);
	this->raw = 0;
	this->port = 0;
}

void ae::net::UdpSocket::send(socket::IpAddress ip, Packet p)
{
	if (this->raw == 0) return;

	sockaddr_in addr;
	i32 size = sizeof(sockaddr_in);
	memset(&addr, 0, size);
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr(ip.first.c_str());
	addr.sin_port = htons(ip.second);

	i32 result = sendto(
		this->raw, (const char*)p.buf, p.len, 0,
		(sockaddr*)&addr, size
	);
	
	if (result == -1)
	{
		printf("UDP %llu: error %i\n", this->raw, socket::getError());
	}
	else if ((usize)result != p.len)
	{
		printf("UDP %llu: sent != length (%i/%i)\n", this->raw, result, p.len);
	}
}

std::pair<ae::socket::IpAddress, ae::net::Packet>
	ae::net::UdpSocket::recv()
{
	sockaddr_in addr;
	i32 size = sizeof(sockaddr_in);
	memset(&addr, 0, size);
	auto buffer = new u8[256];
	auto len = recvfrom(
		this->raw, (char*)buffer, 256, 0,
		(sockaddr*)&addr, &size
	);

	if (len == -1)
	{
		auto status = socket::getError();
		if (
			status != WSAETIMEDOUT &&
			status != WSAEINTR
		) { printf("UDP error: %i\n", socket::getError()); }
		return { socket::IpAddress { "", 0 }, Packet { nullptr, 0 } };
	}

	return {
		socket::IpAddress
		{
			inet_ntoa(addr.sin_addr),
			ntohs(addr.sin_port)
		},
		Packet { buffer, (usize)len }
	};
}

void ae::net::UdpSocket::setBroadcast(bool active)
{
	if (this->raw == 0) return;
	char enable = active ? '1' : '0';
	printf("setBroadcast: %i\n", setsockopt(this->raw, SOL_SOCKET,
		SO_BROADCAST, &enable, 1
	));
}

void ae::net::UdpSocket::setTimeout(float send, float recv)
{
	if (this->raw == 0) return;
	send *= 1000; recv *= 1000;
	timeval s, r;
	s.tv_sec = floor(send); s.tv_usec = (i32)((send - s.tv_sec) * 1e6);
	r.tv_sec = floor(recv); r.tv_usec = (i32)((recv - r.tv_sec) * 1e6);
	printf("setRecvTimeout: %i\n", setsockopt(
		this->raw, SOL_SOCKET, SO_RCVTIMEO,
		(char*)&r, sizeof(timeval)
	));
	printf("setSendTimeout: %i\n", setsockopt(
		this->raw, SOL_SOCKET, SO_SNDTIMEO,
		(char*)&s, sizeof(timeval)
	));
}

ae::socket::Socket ae::net::UdpSocket::getSocket() { return this->raw; }
ae::u16 ae::net::UdpSocket::getPort() { return this->port; }

void ae::net::UdpSocket::init()
{
	if (this->raw != 0) { closesocket(this->raw); }

	this->raw = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (this->raw == INVALID_SOCKET)
	{
		printf("Failed to create socket: %i\n",
			socket::getError()
		);
		this->raw = 0;
	}
	this->port = 0;
}