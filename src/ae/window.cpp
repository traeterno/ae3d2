#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ae/window.hpp>
#include <ae/global.hpp>
#include <ae/network.hpp>

using namespace ae;

void errorCallback(int id, const char* description)
{
	printf("GLFW Error #(%i): %s", id, description);
}

void resizeCallback(GLFWwindow* win, int w, int h)
{
	glViewport(0, 0, w, h);
	Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(win));
	window->getUI()->resized();
	window->getCamera()->resized();
}

void keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods)
{
	Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(win));
	window->key = KeyEvent { .key = key, .mods = mods, .action = action };
}

UI* Window::getUI() { return &this->ui; }
GLFWwindow* Window::getGLFW() { return this->window; }
glm::vec2 Window::getBaseSize() { return this->uiSize; }
Camera* Window::getCamera() { return &this->cam; }

Window::~Window()
{
	printf("Closing the window\n");
	if (this->window != nullptr) glfwDestroyWindow(this->window);
	glfwTerminate();
}

Window::Window(std::string path, int argc, char* argv[]):
	cam(Camera(this)),
	ui(UI(this))
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
		root["main"]["uiSize"][0].asFloat(),
		root["main"]["size"][1].asFloat()
	);

	int width = root["main"]["size"][0].asInt();
	int height = root["main"]["size"][1].asInt();

	this->window = glfwCreateWindow(
		width, height, root["main"]["title"].asCString(),
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

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to load OpenGL\n");
		exit(0);
	}

	printf("Version: %s\nVendor: %s\n",
		glGetString(GL_VERSION), glGetString(GL_VENDOR)
	);

	printf("Created the window\n");
	glViewport(0, 0, width, height);
	ae::net::init();
	if (!this->cam.init())
	{
		printf("Failed to create the camera\n");
		exit(0);
	}
	auto ft = this->cam.setFont(root["main"]["font"].asCString());
	this->font.load(root["main"]["font"].asCString(), ft);
	if (!this->ui.load(root["main"]["ui"].asCString()))
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
	ae::net::shutdown();
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
	this->cam.display();
	this->ui.render();
	this->display();
}

bool Window::keyPressed(std::string key)
{
	return glfwGetKey(this->window, ae::input::str2key(key)) == 1;
}

void Window::update()
{
	this->key = {0, 0, 0};
	glfwPollEvents();
	auto p = hrc::now();
	constexpr f32 scaler = 1e-6;
	this->deltaTime = (f32)(std::chrono::duration_cast<std::chrono::microseconds>
		(p - this->deltaTimer).count()) * scaler;
	this->deltaTimer = p;
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

text::Font* Window::getFont()
{
	return &this->font;
}