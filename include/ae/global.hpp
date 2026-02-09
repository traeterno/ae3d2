#ifndef aeGlobal
#define aeGlobal

#include "ae/types.hpp"
#include <string>
#include <json/value.h>
#include <glm/fwd.hpp>

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

namespace ae::math
{
	glm::quat buildQuat(float yaw, float pitch, float roll, bool global);
}

#endif