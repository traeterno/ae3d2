#ifndef aeCamera
#define aeCamera

#include <unordered_map>
#include <ae/types.hpp>
#include <glm/glm.hpp>

namespace ae
{

class Window;

struct Texture { u32 id; u32 width; u32 height; };

struct Offscreen { u32 fbo, tex, depth, vao; };

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
	void useProjection(bool perspective);
	void useView(bool camera);
	void shaderUse(const char* id);
	void shaderMat4(const char* uniform, glm::mat4 value);
	void shaderVec2(const char* uniform, glm::vec2 value);
	void shaderVec4(const char* uniform, glm::vec4 value);
	void shaderInt(const char* uniform, i32 value);
	Texture getTexture(const char* id);
private:
	std::unordered_map<const char*, Texture> textures;
	std::unordered_map<const char*, u32> shaders;
	Window* window;
	Offscreen offscreen;
	u32 currentShader, currentTexture, currentVAO;
	u32 spriteVAO;
	// HINT add meshVAO; for drawing meshes, on draw() calls bind VBO+EBO
	glm::mat4 perspective, orthographic, currentProj;
	glm::mat4 camView, currentView;
};

}

#endif