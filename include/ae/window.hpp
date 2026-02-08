#ifndef aeWindow
#define aeWindow

#include <string>
#include <GLFW/glfw3.h>
#include <ae/ui.hpp>
#include <glm/glm/glm.hpp>
#include <ae/types.hpp>
#include <ae/camera.hpp>

namespace ae
{

struct KeyEvent { i32 key; i32 mods; i32 action; };

class Window
{
public:
	void close();
	bool isOpen();
	void update();
	void clear();
	void display();
	void render();
	bool keyPressed(std::string key);
	glm::vec2 getSize();

	Window(std::string cfg, int argc, char* argv[]);
	~Window();
	UI* getUI();
	GLFWwindow* getGLFW();
	glm::vec2 getBaseSize();
	Camera* getCamera();
	KeyEvent key;
private:
	glm::vec2 uiSize;
	GLFWwindow* window;
	UI ui;
	Camera cam;
};

}

#endif