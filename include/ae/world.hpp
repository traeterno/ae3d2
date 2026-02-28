#ifndef aeWorld
#define aeWorld

#include "ae/mesh.hpp"
#include <string>
#include <unordered_map>
extern "C"
{
	#include <lua/lua.h>
	#include <lua/lauxlib.h>
	#include <lua/lualib.h>
}

namespace ae { class Window; };

namespace ae::world
{

class Entity
{
public:
	Entity(Window* win, const char* id, const char* path);
	~Entity();
	bool init();
	bool update();
	bool render();
	mesh::Mesh* getMesh();
	mesh::Skeleton* getSkeleton();
private:
	lua_State* state;
	mesh::Mesh mesh;
	mesh::Skeleton skeleton;
	Window* window;
};

class World
{
public:
	World(Window* win);
	~World();
	void init();
	bool load(const char* name);
	Entity* getEntity(const char* name);
	void update();
	void render();
	void requestReload(const char* name);
	void spawn(const char* id, const char* path);
private:
	std::unordered_map<std::string, Entity*> ents;
	lua_State* state;
	std::string reload;
	Window* window;
	bool ready;
};

}

#endif