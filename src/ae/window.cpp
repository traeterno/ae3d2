#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ae/window.hpp>
#include <ae/global.hpp>
#include <cstdio>

using namespace ae;

Window::~Window()
{
	Window::log("Closing the window");
	if (this->window != nullptr) glfwDestroyWindow(this->window);
	glfwTerminate();
}

Window::Window(std::string path, int argc, char* argv[])
{
	auto root = ae::fs::readJSON(path);
	if (root.empty()) Window::error("The configuration file is empty");

	this->logging = root["main"]["log"].asBool();
	if (this->logging) fclose(fopen("res/log.txt", "w"));

	Window::log("Creating the window");

	if (!glfwInit()) Window::error("Failed to start GLFW");

	int width = root["main"]["size"][0].asInt();
	int height = root["main"]["size"][1].asInt();

	this->window = glfwCreateWindow(
		width, height, root["main"]["title"].asCString(),
		nullptr, nullptr
	);
	if (!this->window) Window::error("Failed to create the window");

	glfwMakeContextCurrent(this->window);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		Window::error("Failed to load OpenGL");
	}

	glfwSetFramebufferSizeCallback(this->window,
	[](GLFWwindow* win, int w, int h)
	{ glViewport(0, 0, w, h); });

	glViewport(0, 0, width, height);
	glClearColor(0.5, 0.5, 0.5, 1.0);

	Window::log("Created the window");
}

void Window::log(std::string s)
{
	if (this->logging)
	{
		auto f = fopen("res/log.txt", "a");
		fprintf(f, "%s\n", s.c_str());
		fclose(f);
	}
}

void Window::error(std::string s)
{
	Window::log(s);
	exit(1);
}

void Window::close()
{
	glfwSetWindowShouldClose(this->window, true);
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