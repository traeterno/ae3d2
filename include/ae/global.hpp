#ifndef aeGlobal
#define aeGlobal

#include "ae/types.hpp"
#include <string>
#include <json/value.h>

extern "C"
{
	#include <lua/lua.h>
}

namespace ae::fs
{
	std::string readText(std::string path);
	Json::Value readJSON(std::string path);
}

namespace ae::str
{
	void removeAll(std::string& base, std::string part);

	std::string format(const char* style, ...);
}

namespace ae::script
{
	bool execute(lua_State* s, const char* code);
	bool runFunction(lua_State* s, const char* name);
}

namespace ae::input
{
	i32 str2key(std::string key);
}

#endif