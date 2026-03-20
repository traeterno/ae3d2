#include <ae/global.hpp>
#include <ae/socket.hpp>
#include <filesystem>
#include <thread>
#include <nlohmann/json.hpp>

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
	this->raw = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
	ip.sin_family = AF_INET;
	ip.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	ip.sin_port = htons(port);
	if (::bind(this->raw, (sockaddr*)&ip, sizeof(sockaddr_in)) == SOCKET_ERROR)
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
	printf("Started listener on port %i\n", port);
}

ae::net::TcpStream ae::net::TcpListener::accept()
{
	sockaddr_in addr;
	printf("Waiting for client...\n");
	socket::Socket raw = ::accept(this->raw, (sockaddr*)&addr, NULL);
	printf("Accepted client %s:%i\n",
		inet_ntoa(addr.sin_addr), ntohs(addr.sin_port)
	);
	return TcpStream(raw, addr);
}

ae::socket::Socket ae::net::TcpListener::getSocket() { return this->raw; }

ae::net::TcpStream::TcpStream()
{
	this->raw = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->raw == INVALID_SOCKET)
	{
		printf("Failed to create socket\n");
		this->raw = 0;
	}
	memset(&this->addr, 0, sizeof(sockaddr_in));
}

ae::net::TcpStream::TcpStream(socket::Socket s, sockaddr_in addr)
{
	this->raw = s;
	this->addr = addr;
}

ae::net::TcpStream::~TcpStream()
{
	if (this->raw != 0)
	{
		closesocket(this->raw);
	}
}

bool ae::net::TcpStream::connect(std::string ip, u16 port)
{
	this->addr.sin_family = AF_INET;
	this->addr.sin_port = htons(port);
	this->addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	printf("Connecting to %s:%i\n", ip.c_str(), port);
	
	if (::connect(this->raw, (sockaddr*)&this->addr, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		printf("Failed to connect\n");
		return false;
	}
	printf("Connected\n");
	return true;
}

void ae::net::TcpStream::disconnect()
{
	::shutdown(this->raw, SD_BOTH);
}

void ae::net::TcpStream::send(Packet p)
{
	auto size = ::send(this->raw, (i8*)p.buf, p.len, 0);
	printf("Sent %i/%i bytes\n", size, p.len);
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