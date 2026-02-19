#ifndef aeCamera
#define aeCamera

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <ae/types.hpp>
#include <glm/glm.hpp>

namespace ae
{

class Window;

struct Texture { u32 id; u32 width; u32 height; };

struct Offscreen { u32 fbo, tex, depth, vao; };

namespace text { class Font; }

class Camera
{
public:
	Camera(Window* win);
	~Camera();
	bool init();
	void clearCache();
	void clear();
	void display();
	void resized();
	void textureUse(const char* id);
	void bindVAO(u32 id);
	void drawSprite();
	void drawText(u32 id, usize len);
	void useProjection(bool perspective);
	void useView(bool camera);
	u32 createVBO();
	void removeVBO(u32 id);
	void shaderUse(const char* id);
	void shaderMat4(const char* uniform, glm::mat4 value);
	void shaderVec2(const char* uniform, glm::vec2 value);
	void shaderVec4(const char* uniform, glm::vec4 value);
	void shaderInt(const char* uniform, i32 value);
	Texture getTexture(const char* id);
	void setFont(const char* name);
	text::Font* getFont();
	void requestClearCache();
private:
	std::unordered_map<const char*, Texture> textures;
	std::unordered_map<const char*, u32> shaders;
	std::unordered_set<u32> VBOs;
	Window* window;
	Offscreen offscreen;
	u32 currentShader, currentTexture, currentVAO;
	u32 spriteVAO, textVAO;
	std::string fontName;
	// HINT add meshVAO; for drawing meshes, on draw() calls bind VBO+EBO
	glm::mat4 perspective, orthographic, currentProj;
	glm::mat4 camView, currentView;
	text::Font* font;
	bool rcc;
};

}

#endif