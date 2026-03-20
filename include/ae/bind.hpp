#ifndef aeBind
#define aeBind

extern "C"
{
	#include <lua/lua.h>
	#include <lua/lauxlib.h>
	#include <lua/lualib.h>
}

namespace ae { class Window; }

namespace ae::bind
{
	void setup(lua_State* script, ae::Window* win, const char* executor);
	void window(lua_State* script);
	void camera(lua_State* script);
	void world(lua_State* script);
	void entity(lua_State* script);
	void network(lua_State* script);
}

#endif