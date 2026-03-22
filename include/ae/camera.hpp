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
namespace gltf { struct GLTF; }

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
	void bindTexture(const char* id);
	void bindTexture(u32 id);
	void useProjection(bool perspective);
	void useView(bool camera);
	u32 createVBO();
	void removeVBO(u32 id);
	Texture getTexture(const char* id);
	void setFont(const char* name);
	text::Font* getFont();
	void requestClearCache();
	void lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up);
	void loadGLTF(const char* id);
	gltf::GLTF* getGLTF(const char* id);
	void unloadGLTF(const char* id);

	void drawSprite(glm::mat4 ts);
	void drawText(u32 id, usize len, glm::mat3 rot, glm::mat4 ts);

	void bindMeshVAO();
	void drawShape(u32 vbo, u8 type, u32 count, glm::mat4 ts);

	void shaderUse(const char* id);
	void shaderMat3(const char* uniform, glm::mat3 value);
	void shaderMat4(const char* uniform, glm::mat4 value);
	void shaderVec2(const char* uniform, glm::vec2 value);
	void shaderVec3(const char* uniform, glm::vec3 value);
	void shaderVec4(const char* uniform, glm::vec4 value);
	void shaderInt(const char* uniform, i32 value);
	void shaderSetModel(glm::mat4 model);
	i32 shaderGetPos(const char* uniform);
private:
	std::unordered_map<std::string, Texture> textures;
	std::unordered_map<std::string, u32> shaders;
	std::unordered_map<std::string, gltf::GLTF*> gltfs;
	std::unordered_set<u32> VBOs;
	Window* window;
	Offscreen offscreen;
	u32 currentShader, currentTexture, currentVAO;
	u32 spriteVAO, textVAO, meshVAO, shapeVAO;
	std::string fontName;
	glm::mat4 perspective, orthographic, currentProj;
	glm::mat4 camView, currentView;
	text::Font* font;
	bool rcc;
};

}

#endif