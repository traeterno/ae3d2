#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ae/window.hpp>
#include <ae/global.hpp>
#include <ae/network.hpp>

using namespace ae;

i32 str2key(std::string);

void errorCallback(int id, const char* description)
{
	printf("GLFW Error #(%i): %s", id, description);
}

void resizeCallback(GLFWwindow* win, int w, int h)
{
	glViewport(0, 0, w, h);
	Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(win));
	window->getUI()->resized();
}

Window::~Window()
{
	printf("Closing the window\n");
	if (this->window != nullptr) glfwDestroyWindow(this->window);
	glfwTerminate();
}

Window::Window(std::string path, int argc, char* argv[]): ui(UI(this))
{
	auto root = ae::fs::readJSON(path);
	if (root.empty()) { printf("The configuration file is empty"); exit(0); }

	glfwSetErrorCallback(errorCallback);
	if (!glfwInit())
	{
		printf("Failed to start GLFW");
		exit(0);
	}

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
	
	glfwSwapInterval(1);
	glfwMakeContextCurrent(this->window);
	glfwSetWindowUserPointer(this->window, this);
	glfwSetFramebufferSizeCallback(this->window, resizeCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to load OpenGL\n");
		exit(0);
	}

	printf("Created the window\n");
	glViewport(0, 0, width, height);
	ae::net::init();
	if (!this->ui.load(root["main"]["ui"].asCString()))
	{
		printf("Can't load the UI; Stopping the engine\n");
		exit(0);
	}
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
	this->ui.render();
	this->display();
}

bool Window::keyPressed(std::string key)
{
	return glfwGetKey(this->window, str2key(key)) == 1;
}

void Window::update()
{
	glfwPollEvents();
	this->ui.update();
}

UI* Window::getUI() { return &this->ui; }

GLFWwindow* Window::getGLFW() { return this->window; }

glm::vec2 Window::getBaseSize() { return this->uiSize; }

i32 str2key(std::string key)
{
	if (key.length() == 1 && key[0] >= 65 && key[0] <= 90) { return key[0]; } // Latin
	if (key.substr(0, 3) == "Num") { return key[3]; } // Digits
	if (key[0] == 'F')
	{
		if (key.length() == 2) { return GLFW_KEY_F1 + key[1] - 49; }
		if (key.length() == 3)
		{
			return GLFW_KEY_F1 - 1 + std::stoi(key.substr(1, 2));
		} 
	}
	if (key == "Up") return GLFW_KEY_UP;
	if (key == "Down") return GLFW_KEY_DOWN;
	if (key == "Left") return GLFW_KEY_LEFT;
	if (key == "Right") return GLFW_KEY_RIGHT;
	if (key == "Escape") return GLFW_KEY_ESCAPE;
	if (key == "Enter") return GLFW_KEY_ENTER;
	if (key == "Backspace") return GLFW_KEY_BACKSPACE;
	if (key == "Space") return GLFW_KEY_SPACE;
	if (key == "LAlt") return GLFW_KEY_LEFT_ALT;
	if (key == "LShift") return GLFW_KEY_LEFT_SHIFT;
	if (key == "LCtrl") return GLFW_KEY_LEFT_CONTROL;
	if (key == "RAlt") return GLFW_KEY_RIGHT_ALT;
	if (key == "RShift") return GLFW_KEY_RIGHT_SHIFT;
	if (key == "RCtrl") return GLFW_KEY_RIGHT_CONTROL;
	if (key == "Minus") return GLFW_KEY_MINUS;
	if (key == "Equal") return GLFW_KEY_EQUAL;
	if (key == "Tab") return GLFW_KEY_TAB;
	return GLFW_KEY_LAST;
}