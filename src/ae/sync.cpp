#if defined(__WIN32__) or defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <afunix.h>

#endif

#include <ae/sync.hpp>
#include <ae/global.hpp>

#include <math.h>
#include <thread>
#include <filesystem>

#include <nlohmann/json.hpp>

#if defined(__WIN32__) or defined(_WIN32)
void ae::sync::setBlocking(ae::sync::Socket s, bool blocking)
{
	unsigned long mode = blocking ? 0 : 1;
	ioctlsocket(s, FIONBIO, &mode);
}
#endif

std::string getPath()
{
	std::string temp = std::filesystem::temp_directory_path().string() + "ae3d";
	if (!std::filesystem::exists(temp))
	{
		std::filesystem::create_directory(temp);
	}
	return ae::str::format("%s/aeSock%i", temp.c_str(), rand() % 26225);
}

ae::sync::Socket* ae::sync::buildPair()
{
	auto path = getPath();
	::unlink(path.c_str());
	sockaddr_un addr;
	memset(&addr, 0, sizeof(sockaddr_un));
	addr.sun_family = AF_UNIX;
	strcpy_s(addr.sun_path, path.c_str());

	auto server = socket(AF_UNIX, SOCK_STREAM, 0);
	if (server == INVALID_SOCKET)
	{
		printf("Failed to create sync server socket\n");
		return nullptr;
	}

	if (bind(server, (sockaddr*)&addr, sizeof(sockaddr_un)) == SOCKET_ERROR)
	{
		printf("Failed to bind sync server socke\n");
		return nullptr;
	}

	if (listen(server, 0) != 0)
	{
		printf("Failed to listen to sync server socke\n");
	}

	auto pair = new ae::sync::Socket[2];
	pair[0] = 0; pair[1] = 0;
	std::thread t([&pair, &path]()
	{
		sockaddr_un addr;
		memset(&addr, 0, sizeof(sockaddr_un));
		addr.sun_family = AF_UNIX;
		strcpy_s(addr.sun_path, path.c_str());
		pair[0] = socket(AF_UNIX, SOCK_STREAM, 0);
		if (connect(pair[0], (sockaddr*)&addr, sizeof(sockaddr_un)) == SOCKET_ERROR)
		{
			printf("Failed to connect to sync server socket\n");
			pair[0] = -1;
			return;
		}
	});
	pair[1] = accept(server, NULL, NULL);
	t.join();
	if (pair[0] < 0 || pair[1] < 0) { delete[] pair; return nullptr; }
	setBlocking(pair[0], false);
	setBlocking(pair[1],  false);
	return pair;
}

void ae::sync::send(Socket s, json v)
{
	std::string out = v.dump();
	::send(s, out.c_str(), out.length(), 0);
}

json ae::sync::recv(Socket s)
{
	auto buf = new char[64*1024];
	auto len = ::recv(s, buf, 64*1024, 0);
	if (len == -1)
	{
		delete[] buf;
		return json();
	}
	buf[len] = 0;
	auto v = json::parse(buf);
	delete[] buf;
	return v;
}