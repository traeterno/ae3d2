#include <ae/window.hpp>

using namespace ae;

int main(int argc, char* argv[])
{
	Window win("res/game.json", argc, argv);

	while (win.isOpen())
	{
		win.update();
		win.render();
	}
	return 0;
}