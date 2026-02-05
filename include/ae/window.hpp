#ifndef aeWindow
#define aeWindow

#include <string>
#include <GLFW/glfw3.h>

namespace ae { class Window
{
public:
	Window(std::string cfg, int argc, char* argv[]);
	void log(std::string s);
	void error(std::string s);
	void close();
	bool isOpen();
	void update();
	void clear();
	void display();
	~Window();
private:
	bool logging;
	GLFWwindow* window;
}; }

#endif