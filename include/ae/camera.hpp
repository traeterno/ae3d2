#ifndef aeCamera
#define aeCamera

#include <unordered_map>
#include <ae/types.hpp>

namespace ae
{

class Window;

struct Texture { u32 id; u32 width; u32 height; };

struct Offscreen { u32 fbo, tex, depth, vbo, vao; };

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
	void shaderUse(const char* id);
	void textureUse(const char* id);
private:
	u32 loadTexture(const char* id);
	u32 loadShader(const char* id);
	std::unordered_map<const char*, Texture> textures;
	std::unordered_map<const char*, u32> shaders;
	Window* window;
	Offscreen offscreen;
	u32 currentShader, currentTexture;
};

}

#endif