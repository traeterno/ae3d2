#include <glad/glad.h>

#include <ae/camera.hpp>
#include <ae/window.hpp>
#include <ae/global.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm/glm.hpp>

#include <stdio.h>

using namespace ae;

Camera::Camera(Window* win)
{
	this->window = win;
	this->clearCache();
	this->currentShader = 0;
	this->currentTexture = 0;
}

Camera::~Camera()
{
	glDeleteFramebuffers(1, &this->offscreen.fbo);
	glDeleteTextures(1, &this->offscreen.tex);
	glDeleteTextures(1, &this->offscreen.depth);
	printf("Destroying camera\n");
}

bool Camera::init()
{
	this->offscreen = {0, 0, 0, 0, 0};
	glGenFramebuffers(1, &this->offscreen.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, this->offscreen.fbo);

	glGenTextures(1, &this->offscreen.tex);
	glBindTexture(GL_TEXTURE_2D, this->offscreen.tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenTextures(1, &this->offscreen.depth);
	glBindTexture(GL_TEXTURE_2D, this->offscreen.depth);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	this->resized();

	printf("Framebuffer: %s\n",
		glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE ?
		"READY" : "FAILED"
	);

	float vertices[] = {
		-1, -1,
		1, -1,
		1, 1,
		-1, 1,
	};

	glGenVertexArrays(1, &this->offscreen.vao);
	glBindVertexArray(this->offscreen.vao);
	glGenBuffers(1, &this->offscreen.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->offscreen.vbo);
	glBufferData(
		GL_ARRAY_BUFFER, 8 * sizeof(f32),
		vertices, GL_STATIC_DRAW
	);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, 2, GL_FLOAT, GL_FALSE,
		2 * sizeof(f32), 0
	);
	
	printf("Initialized camera\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void Camera::resized()
{
	auto size = this->window->getSize();
	glBindTexture(GL_TEXTURE_2D, this->offscreen.tex);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGB,
		size.x, size.y, 0, GL_RGB,
		GL_UNSIGNED_BYTE, nullptr
	);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		this->offscreen.tex, 0
	);
	glBindTexture(GL_TEXTURE_2D, this->offscreen.depth);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8,
		size.x, size.y, 0, GL_DEPTH_STENCIL,
		GL_UNSIGNED_INT_24_8, nullptr
	);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D,
		this->offscreen.depth, 0
	);
}

void Camera::clearCache()
{
	this->shaders.clear();
	this->textures.clear();
}

void Camera::clear()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->offscreen.fbo);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Camera::display()
{
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	this->shaderUse("offscreen");
	glBindVertexArray(this->offscreen.vao);
	glBindTexture(GL_TEXTURE_2D, this->offscreen.tex);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Camera::shaderUse(const char* name)
{
	auto x = this->shaders.find(name);
	u32 newShader;
	if (x == this->shaders.end())
	{
		auto id = this->loadShader(name);
		if (id == 0) { return; }
		printf("Loaded shader \"%s\" as #(%i)\n", name, id);
		this->shaders.insert({ name, id });
		newShader = id;
	}
	else { newShader = x->second; }
	if (newShader == this->currentShader) return;
	this->currentShader = newShader;
	glUseProgram(this->currentShader);
}

void Camera::textureUse(const char* id)
{
	// TODO
}

u32 Camera::loadShader(const char* name)
{
	i32 success; char infoLog[512];

	auto vpath = ae::str::format("res/shaders/%s.vert", name);
	auto vstr = ae::fs::readText(vpath);
	auto vsrc = vstr.c_str(); i32 vlen = vstr.length();
	auto vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshader, 1, &vsrc, &vlen);
	glCompileShader(vshader);
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vshader, 512, NULL, infoLog);
		printf("Failed to build vertex shader %s: %s\n", name, infoLog);
		glDeleteShader(vshader);
		return 0;
	}
	
	auto fpath = ae::str::format("res/shaders/%s.frag", name);
	auto fstr = ae::fs::readText(fpath);
	auto fsrc = fstr.c_str(); i32 flen = fstr.length();
	auto fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader, 1, &fsrc, &flen);
	glCompileShader(fshader);
	glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fshader, 512, NULL, infoLog);
		printf("Failed to build fragment shader %s: %s\n", name, infoLog);
		glDeleteShader(vshader);
		glDeleteShader(fshader);
		return 0;
	}

	u32 program = glCreateProgram();
	glAttachShader(program, vshader);
	glAttachShader(program, fshader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fshader, 512, NULL, infoLog);
		printf("Failed to link shader %s: %s\n", name, infoLog);
		glDeleteShader(vshader);
		glDeleteShader(fshader);
		glDeleteProgram(program);
		return 0;
	}
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	return program;
}

u32 Camera::loadTexture(const char* name)
{
	// TODO
	return 0;
}