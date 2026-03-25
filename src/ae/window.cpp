#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ae/window.hpp>
#include <ae/global.hpp>
#include <ae/socket.hpp>
#include <nlohmann/json.hpp>

using namespace ae;

void errorCallback(int id, const char* description)
{
	printf("GLFW Error #(%i): %s", id, description);
}

void resizeCallback(GLFWwindow* win, int w, int h)
{
	if (w == 0 || h == 0) return;
	glViewport(0, 0, w, h);
	auto window = (Window*)glfwGetWindowUserPointer(win);
	window->getUI()->resized();
	window->getCamera()->resized();
}

void keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods)
{
	auto window = (Window*)glfwGetWindowUserPointer(win);
	window->key = KeyEvent { .key = key, .mods = mods, .action = action };
}

void mouseCallback(GLFWwindow* win, int button, int action, int mods)
{
	auto window = (Window*)glfwGetWindowUserPointer(win);
	window->mouse = MouseEvent { .btn = button, .action = action, .mods = mods };
}

void scrollCallback(GLFWwindow* win, double x, double y)
{
	auto window = (Window*)glfwGetWindowUserPointer(win);
	window->scroll = {x, y};
}

void charCallback(GLFWwindow* win, ae::u32 ch)
{
	auto window = (Window*)glfwGetWindowUserPointer(win);
	window->codepoint = ch;
}

UI* Window::getUI() { return &this->ui; }
GLFWwindow* Window::getGLFW() { return this->window; }
glm::vec2 Window::getBaseSize() { return this->uiSize; }
Camera* Window::getCamera() { return &this->cam; }
ae::NetworkClient* Window::getNC() { return &this->net; }

Window::~Window()
{
	printf("Closing the window\n");
	if (this->window != nullptr) glfwDestroyWindow(this->window);
	glfwTerminate();
	ae::socket::shutdown();
}

Window::Window(std::string path, int argc, char* argv[]):
	cam(Camera(this)),
	ui(UI(this)),
	world(world::World(this)),
	net(NetworkClient(this))
{
	auto root = ae::fs::readJSON(path);
	if (root.empty()) { printf("The configuration file is empty"); exit(0); }

	glfwSetErrorCallback(errorCallback);
	if (!glfwInit())
	{
		printf("Failed to start GLFW");
		exit(0);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	this->uiSize = glm::vec2(
		root["main"]["uiSize"][0],
		root["main"]["size"][1]
	);

	int width, height;
	if (root["main"]["fullscreen"])
	{
		auto monitor = glfwGetPrimaryMonitor();
		auto vm = glfwGetVideoMode(monitor);
		width = vm->width;
		height = vm->height;
		glfwWindowHint(GLFW_RED_BITS, vm->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, vm->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, vm->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, vm->refreshRate);
		glfwWindowHint(GLFW_DECORATED, 0);
	}
	else
	{
		width = root["main"]["size"][0];
		height = root["main"]["size"][1];
	}
	
	std::string title = root["main"]["title"];
	this->window = glfwCreateWindow(
		width, height, title.c_str(),
		nullptr, nullptr
	);

	if (!this->window)
	{
		printf("Failed to create the window");
		exit(0);
	}
	
	glfwMakeContextCurrent(this->window);
	glfwSwapInterval(1);
	glfwSetWindowUserPointer(this->window, this);
	glfwSetFramebufferSizeCallback(this->window, resizeCallback);
	glfwSetKeyCallback(this->window, keyCallback);
	glfwSetScrollCallback(this->window, scrollCallback);
	glfwSetMouseButtonCallback(this->window, mouseCallback);
	glfwSetCharCallback(this->window, charCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to load OpenGL\n");
		exit(0);
	}

	printf("Version: %s\nVendor: %s\nGLSL version: %s\nRenderer: %s\n",
		glGetString(GL_VERSION), glGetString(GL_VENDOR),
		glGetString(GL_SHADING_LANGUAGE_VERSION), glGetString(GL_RENDERER)
	);

	printf("Created the window\n");
	glViewport(0, 0, width, height);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	ae::socket::init();
	if (!this->cam.init())
	{
		printf("Failed to create the camera\n");
		exit(0);
	}
	std::string font = root["main"]["font"];
	this->cam.setFont(font.c_str());
	if (!this->ui.load(root["main"]["ui"]))
	{
		printf("Can't load the UI; Stopping the engine\n");
		exit(0);
	}
	this->deltaTimer = hrc::now();
	this->deltaTime = 0.1f;
}

void Window::close()
{
	glfwSetWindowShouldClose(this->window, true);
}

bool Window::isOpen()
{
	return !glfwWindowShouldClose(this->window);
}

void Window::clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void Window::display()
{
	glfwSwapBuffers(this->window);
}

void Window::render()
{
	this->clear();
	this->cam.clear();
	this->world.render();
	this->cam.display();
	this->ui.render();
	this->display();
}

bool Window::keyPressed(std::string key)
{
	return glfwGetKey(this->window, ae::input::str2key(key)) == GLFW_PRESS;
}

bool Window::mousePressed(std::string btn)
{
	return glfwGetMouseButton(window, ae::input::str2btn(btn)) == GLFW_PRESS;
}

void Window::update()
{
	this->key = {0, 0, 0};
	this->mouse = {0, 0, 0};
	this->scroll = {0, 0};
	this->codepoint = 0;
	glfwPollEvents();
	auto p = hrc::now();
	constexpr f32 scaler = 1e-6;
	this->deltaTime = (f32)(std::chrono::duration_cast<std::chrono::microseconds>
		(p - this->deltaTimer).count()) * scaler;
	this->deltaTime = glm::clamp(this->deltaTime, 0.001f, 0.1f);
	this->deltaTimer = p;
	this->cam.clearCache();
	this->world.update();
	this->ui.update();
}

glm::vec2 Window::getSize()
{
	i32 w, h;
	glfwGetWindowSize(this->window, &w, &h);
	return glm::vec2(w, h);
}

f32 Window::getDeltaTime()
{
	return this->deltaTime;
}

world::World* Window::getWorld()
{
	return &this->world;
}

glm::vec2 Window::mousePos()
{
	double x, y;
	glfwGetCursorPos(this->window, &x, &y);
	if (glfwGetWindowAttrib(this->window, GLFW_MAXIMIZED)) y += 0.5;
	return glm::vec2(x, y);
}