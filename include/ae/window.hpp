#ifndef aeWindow
#define aeWindow

#include <string>
#include <GLFW/glfw3.h>
#include <ae/ui.hpp>
#include <glm/glm.hpp>
#include <ae/types.hpp>
#include <ae/font.hpp>
#include <ae/camera.hpp>
#include <chrono>

using hrc = std::chrono::high_resolution_clock;

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
	f32 getDeltaTime();

	Window(std::string cfg, int argc, char* argv[]);
	~Window();
	UI* getUI();
	GLFWwindow* getGLFW();
	glm::vec2 getBaseSize();
	Camera* getCamera();
	text::Font* getFont();
	KeyEvent key;
private:
	std::chrono::time_point<hrc> deltaTimer;
	f32 deltaTime;
	glm::vec2 uiSize;
	GLFWwindow* window;
	Camera cam;
	UI ui;
	text::Font font;
};

}

#endif