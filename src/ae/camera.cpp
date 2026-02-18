#include <glad/glad.h>

#include <ae/camera.hpp>
#include <ae/window.hpp>
#include <ae/global.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>

using namespace ae;

u32 loadShader(const char* name);
Texture loadTexture(const char* name);

Camera::Camera(Window* win)
{
	this->window = win;
	this->clearCache();
	this->currentShader = 0;
	this->currentTexture = 0;
	this->currentVAO = 0;
	this->spriteVAO = 0;
	this->textVAO = 0;
	this->textInit = false;
	this->currentProj = glm::mat4(1.0);
	this->camView = glm::mat4(1.0);
	this->currentView = glm::mat4(1.0);
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
	this->offscreen = {0, 0, 0, 0};
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
	u32 offscreenVBO;
	glGenBuffers(1, &offscreenVBO);
	glBindBuffer(GL_ARRAY_BUFFER, offscreenVBO);
	glBufferData(
		GL_ARRAY_BUFFER, 8 * sizeof(f32),
		vertices, GL_STATIC_DRAW
	);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, 2, GL_FLOAT, GL_FALSE,
		2 * sizeof(f32), 0
	);

	vertices[0] = 0; vertices[1] = 0;
	vertices[3] = 0; vertices[6] = 0;

	glGenVertexArrays(1, &this->spriteVAO);
	glBindVertexArray(this->spriteVAO);
	u32 spriteVBO;
	glGenBuffers(1, &spriteVBO);
	glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
	glBufferData(
		GL_ARRAY_BUFFER, 8 * sizeof(f32),
		vertices, GL_STATIC_DRAW
	);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, 2, GL_FLOAT, GL_FALSE,
		2 * sizeof(f32), 0
	);

	glGenVertexArrays(1, &this->textVAO);
	
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

	this->perspective = glm::perspectiveLH(
		glm::radians(70.0f),
		size.x / size.y,
		0.01f, 1000.0f
	);
	this->orthographic = glm::ortho(
		0.0f, size.x, size.y, 0.0f
	);
}

void Camera::clearCache()
{
	for (auto s : this->shaders) glDeleteProgram(s.second);
	for (auto t : this->textures) glDeleteTextures(1, &t.second.id);
	for (auto vbo : this->VBOs) glDeleteBuffers(1, &vbo);
	this->shaders.clear();
	this->textures.clear();
	this->VBOs.clear();
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
	this->bindVAO(this->offscreen.vao);
	this->currentTexture = this->offscreen.tex;
	glBindTexture(GL_TEXTURE_2D, this->offscreen.tex);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Camera::drawSprite()
{
	this->bindVAO(this->spriteVAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Camera::drawText(u32 id, usize len)
{
	this->bindVAO(this->textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	if (!this->textInit)
	{
		this->textInit = true;
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0, 4, GL_FLOAT, GL_FALSE,
			4 * sizeof(f32), 0
		);
	}
	this->shaderUse("text");
	this->textureUse(this->fontName.c_str());
	glDrawArrays(GL_TRIANGLES, 0, len);
}

Texture Camera::setFont(const char* name)
{
	this->fontName = ae::str::format("fonts/%s", name);
	return this->getTexture(this->fontName.c_str());
}

u32 Camera::createVBO()
{
	u32 vbo;
	glGenBuffers(1, &vbo);
	this->VBOs.insert(vbo);
	return vbo;
}

void Camera::removeVBO(u32 id)
{
	auto t = this->VBOs.find(id);
	if (t != this->VBOs.end())
	{
		glDeleteBuffers(1, &id);
		this->VBOs.erase(t);
	}
}

void Camera::bindVAO(u32 id)
{
	if (id == this->currentVAO) return;
	this->currentVAO = id;
	glBindVertexArray(this->currentVAO);
}

void Camera::useProjection(bool p)
{
	this->currentProj = p ? this->perspective : this->orthographic;
}

void Camera::useView(bool cam)
{
	this->currentProj = cam ? this->camView : glm::mat4(1.0);
}

Texture Camera::getTexture(const char* name)
{
	auto t = this->textures.find(name);
	if (t == this->textures.end())
	{
		auto tex = loadTexture(name);
		if (tex.id == 0) { return tex; }
		printf("Loaded texture \"%s\" as #(%i) %ix%i\n",
			name, tex.id, tex.width, tex.height
		);
		this->textures.insert({ name, tex });
		return tex;
	}
	return t->second;
}

void Camera::shaderUse(const char* name)
{
	auto x = this->shaders.find(name);
	u32 newShader;
	if (x == this->shaders.end())
	{
		auto id = loadShader(name);
		if (id == 0) { return; }
		printf("Loaded shader \"%s\" as #(%i)\n", name, id);
		this->shaders.insert({ name, id });
		newShader = id;
	}
	else { newShader = x->second; }
	if (newShader == this->currentShader) return;
	this->currentShader = newShader;
	glUseProgram(this->currentShader);
	this->shaderMat4("projection", this->currentProj);
	this->shaderMat4("view", this->currentView);
}

void Camera::shaderMat4(const char* uniform, glm::mat4 value)
{
	i32 pos = glGetUniformLocation(this->currentShader, uniform);
	if (pos == -1) return;
	glUniformMatrix4fv(
		pos, 1,
		GL_FALSE, glm::value_ptr(value)
	);
}

void Camera::shaderVec2(const char* uniform, glm::vec2 value)
{
	i32 pos = glGetUniformLocation(this->currentShader, uniform);
	if (pos == -1) return;
	glUniform2f(pos, value.x, value.y);
}


void Camera::shaderVec4(const char* uniform, glm::vec4 value)
{
	i32 pos = glGetUniformLocation(this->currentShader, uniform);
	if (pos == -1) return;
	glUniform4f(pos, value.x, value.y, value.z, value.w);
}

void Camera::shaderInt(const char* uniform, i32 value)
{
	i32 pos = glGetUniformLocation(this->currentShader, uniform);;
	if (pos == -1) return;
	glUniform1i(pos, value);
}

void Camera::textureUse(const char* name)
{
	auto t = this->getTexture(name);
	if (this->currentTexture == t.id) return;
	this->currentTexture = t.id;
	glBindTexture(GL_TEXTURE_2D, this->currentTexture);
}

u32 loadShader(const char* name)
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

Texture loadTexture(const char* name)
{
	int w, h, channels;
	u32 id = 0;
	ae::u8* data = stbi_load(
		ae::str::format("res/tex/%s.png", name).c_str(),
		&w, &h, &channels, 0
	);
	if (data == nullptr)
	{
		printf("Failed to load texture \"%s\"\n", name);
		return {0, 0, 0};
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	auto format = (channels == 4 ? GL_RGBA : GL_RGB);
	glTexImage2D(
		GL_TEXTURE_2D, 0, format,
		w, h, 0,
		format, GL_UNSIGNED_BYTE, data
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
	return { id, (ae::u32)w, (ae::u32)h };
}