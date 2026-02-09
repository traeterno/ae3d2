#ifndef aeUI
#define aeUI

#include <string>

extern "C"
{
	#include <lua/lua.h>
	#include <lua/lauxlib.h>
	#include <lua/lualib.h>
}

namespace ae
{

class Window;
class Camera;

class UI
{
public:
	UI(Window* win);
	~UI();
	bool load(std::string id);
	void requestReload(std::string id);
	void update();
	void render();
	void resized();
private:
	void init();
	lua_State* state;
	Window* window;
	Camera* camera;
	std::string reload;
};
	
}

#endif