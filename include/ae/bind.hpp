#ifndef aeBind
#define aeBind

extern "C"
{
	#include <lua/lua.h>
	#include <lua/lauxlib.h>
	#include <lua/lualib.h>
}

namespace ae::bind
{
	void setup(lua_State* script);
	void window(lua_State* script);
	void camera(lua_State* script);
}

#endif