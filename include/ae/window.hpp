#ifndef aeWindow
#define aeWindow

#include <string>
#include <ae/ui.hpp>
#include <glm/glm.hpp>
#include <ae/types.hpp>
#include <ae/camera.hpp>
#include <ae/world.hpp>
#include <ae/network.hpp>
#include <chrono>

using hrc = std::chrono::high_resolution_clock;

struct GLFWwindow;

namespace ae
{

struct KeyEvent { i32 key; i32 mods; i32 action; };
struct MouseEvent { i32 btn; i32 action; i32 mods; };

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
	bool mousePressed(std::string btn);
	glm::vec2 getSize();
	f32 getDeltaTime();
	glm::vec2 mousePos();

	Window(std::string cfg, int argc, char* argv[]);
	~Window();
	UI* getUI();
	GLFWwindow* getGLFW();
	glm::vec2 getBaseSize();
	Camera* getCamera();
	world::World* getWorld();
	NetworkClient* getNC();
	KeyEvent key;
	MouseEvent mouse;
	glm::vec2 scroll;
	u32 codepoint;
private:
	std::chrono::time_point<hrc> deltaTimer;
	f32 deltaTime;
	glm::vec2 uiSize;
	GLFWwindow* window;
	Camera cam;
	UI ui;
	world::World world;
	NetworkClient net;
};

}

#endif