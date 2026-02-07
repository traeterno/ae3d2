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
	void window(lua_State* script);
	void math(lua_State* script);
}

#endif