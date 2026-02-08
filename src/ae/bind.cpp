#include <ae/bind.hpp>
#include <ae/window.hpp>
#include <glm/glm/glm.hpp>
#include <ae/types.hpp>
#include <ae/global.hpp>

using namespace ae;

#define LUA(f) int ae_##f(lua_State* script)

ae::Window* getWindow(lua_State* script)
{
	lua_getglobal(script, "_winptr");
	auto addr = lua_tointeger(script, -1);
	return reinterpret_cast<ae::Window*>(addr);
}

void insertNumber(lua_State* script, std::string name, float v)
{
	lua_pushstring(script, name.c_str());
	lua_pushnumber(script, v);
	lua_settable(script, -3);
}

void insertFunction(lua_State* script, std::string name, lua_CFunction f)
{
	lua_pushstring(script, name.c_str());
	lua_pushcfunction(script, f);
	lua_settable(script, -3);
}

glm::vec2 lua_vec2(lua_State* script)
{
	lua_getfield(script, -1, "x");
	lua_getfield(script, -1, "y");
	return glm::vec2(
		lua_tonumber(script, -2),
		lua_tonumber(script, -1)
	);
}

void vec2_lua(lua_State* script, glm::vec2 v)
{
	lua_createtable(script, 0, 2);
	insertNumber(script, "x", v.x);
	insertNumber(script, "y", v.y);
}

glm::vec3 lua_vec3(lua_State* script)
{
	lua_getfield(script, -1, "x");
	lua_getfield(script, -2, "y");
	lua_getfield(script, -3, "z");
	return glm::vec3(
		lua_tonumber(script, -3),
		lua_tonumber(script, -2),
		lua_tonumber(script, -1)
	);
}

void vec3_lua(lua_State* script, glm::vec3 v)
{
	lua_createtable(script, 0, 2);
	insertNumber(script, "x", v.x);
	insertNumber(script, "y", v.y);
	insertNumber(script, "z", v.z);
}

LUA(window_close)
{
	auto win = getWindow(script);
	win->close();
	return 0;
}

LUA(window_clearColor)
{
	auto c = lua_vec3(script);
	glClearColor(c.x, c.y, c.z, 1.0);
	return 0;
}

LUA(window_keyPressed)
{
	auto key = lua_tostring(script, -1);
	lua_pushboolean(script, getWindow(script)->keyPressed(key));
	return 1;
}

LUA(window_keyJustPressed)
{
	auto key = lua_tostring(script, -1);
	auto id = ae::input::str2key(key);
	auto e = getWindow(script)->key;
	lua_pushboolean(script, id == e.key && e.action == GLFW_PRESS);
	return 1;
}

LUA(window_loadUI)
{
	auto id = lua_tostring(script, -1);
	getWindow(script)->getUI()->requestReload(id);
	return 0;
}

LUA(window_size)
{
	vec2_lua(script, getWindow(script)->getSize());
	return 1;
}

LUA(window_uiSize)
{
	vec2_lua(script, getWindow(script)->getBaseSize());
	return 1;
}

void ae::bind::window(lua_State *script)
{
	lua_createtable(script, 0, 4);
	insertFunction(script, "close", ae_window_close);
	insertFunction(script, "clearColor", ae_window_clearColor);
	insertFunction(script, "keyPressed", ae_window_keyPressed);
	insertFunction(script, "keyJustPressed", ae_window_keyJustPressed);
	insertFunction(script, "loadUI", ae_window_loadUI);
	insertFunction(script, "size", ae_window_size);
	insertFunction(script, "uiSize", ae_window_uiSize);
	lua_setglobal(script, "window");
}

LUA(math_vec2)
{
	float x = lua_tonumber(script, -2);
	float y = lua_tonumber(script, -1);
	vec2_lua(script, glm::vec2(x, y));
	return 1;
}

LUA(math_vec3)
{
	float x = lua_tonumber(script, -3);
	float y = lua_tonumber(script, -2);
	float z = lua_tonumber(script, -1);
	vec3_lua(script, glm::vec3(x, y, z));
	return 1;
}

void ae::bind::math(lua_State *script)
{
	lua_createtable(script, 0, 2);
	insertFunction(script, "vec2", ae_math_vec2);
	insertFunction(script, "vec3", ae_math_vec3);
	lua_setglobal(script, "aeMath");
}