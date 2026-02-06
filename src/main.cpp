#include <ae/window.hpp>
#include <ae/network.hpp>

#include <ae/global.hpp>

using namespace ae;

int main(int argc, char* argv[])
{
	Window win("res/game.json", argc, argv);

	ae::net::TcpSocket s("127.0.0.1", 26225);

	while (win.isOpen())
	{
		win.update();
		win.clear();
		win.display();
	}
	return 0;
}