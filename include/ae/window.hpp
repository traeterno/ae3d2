#ifndef aeWindow
#define aeWindow

#include <string>
#include <GLFW/glfw3.h>
#include <ae/ui.hpp>
#include <glm/glm/glm.hpp>
#include <ae/types.hpp>

namespace ae
{

class Window
{
public:
	Window(std::string cfg, int argc, char* argv[]);
	void close();
	bool isOpen();
	void update();
	void clear();
	void display();
	void render();
	bool keyPressed(std::string key);
	~Window();
	UI* getUI();
	GLFWwindow* getGLFW();
	glm::vec2 getBaseSize();
private:
	i32 keyEvent;
	glm::vec2 uiSize;
	GLFWwindow* window;
	UI ui;
};

}

#endif