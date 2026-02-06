#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ae/window.hpp>
#include <ae/global.hpp>
#include <ae/network.hpp>

using namespace ae;
using ae::str::format;

Window::~Window()
{
	printf("Closing the window");
	if (this->window != nullptr) glfwDestroyWindow(this->window);
	glfwTerminate();
}

Window::Window(std::string path, int argc, char* argv[])
{
	auto root = ae::fs::readJSON(path);
	if (root.empty()) { printf("The configuration file is empty"); exit(0); }

	this->logging = root["main"]["log"].asBool();
	if (this->logging) fclose(fopen("res/log.txt", "w"));

	printf("Creating the window\n");

	int result;
	if (!(result = glfwInit()))
	{
		printf("Failed to start GLFW: %i", result);
		exit(0);
	}

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
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to load OpenGL");
		exit(0);
	}

	glfwSetFramebufferSizeCallback(this->window,
	[](GLFWwindow* win, int w, int h)
	{ glViewport(0, 0, w, h); });

	glViewport(0, 0, width, height);
	glClearColor(0.5, 0.5, 0.5, 1.0);

	ae::net::init();

	printf("Created the window\n");
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

void Window::update()
{
	glfwPollEvents();
}

void Window::clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void Window::display()
{
	glfwSwapBuffers(this->window);
}