#include <glad/glad.h>
#include <ae/bind.hpp>
#include <ae/window.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>
#include <ae/types.hpp>
#include <ae/global.hpp>
#include <ae/font.hpp>

using namespace ae;

#define LUA(f) int ae_##f(lua_State* script)

void ae::bind::setup(lua_State* script)
{
	lua_getglobal(script, "package");
	lua_pushstring(script, "path");
	lua_pushstring(script, "./res/scripts/?.lua");
	lua_settable(script, -3);
}

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

void insertBoolean(lua_State* script, std::string name, bool f)
{
	lua_pushstring(script, name.c_str());
	lua_pushboolean(script, f);
	lua_settable(script, -3);
}

glm::vec2 lua_vec2(lua_State* script)
{
	lua_getfield(script, -1, "x");
	lua_getfield(script, -2, "y");
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
	lua_createtable(script, 0, 3);
	insertNumber(script, "x", v.x);
	insertNumber(script, "y", v.y);
	insertNumber(script, "z", v.z);
}

glm::vec4 lua_vec4(lua_State* script)
{
	lua_getfield(script, -1, "x");
	lua_getfield(script, -2, "y");
	lua_getfield(script, -3, "z");
	lua_getfield(script, -4, "w");
	return glm::vec4(
		lua_tonumber(script, -4),
		lua_tonumber(script, -3),
		lua_tonumber(script, -2),
		lua_tonumber(script, -1)
	);
}

void vec4_lua(lua_State* script, glm::vec4 v)
{
	lua_createtable(script, 0, 4);
	insertNumber(script, "x", v.x);
	insertNumber(script, "y", v.y);
	insertNumber(script, "z", v.z);
	insertNumber(script, "w", v.w);
}

glm::quat lua_quat(lua_State* script)
{
	lua_getfield(script, -1, "yaw");
	lua_getfield(script, -2, "pitch");
	lua_getfield(script, -3, "roll");
	lua_getfield(script, -4, "relative");
	return ae::math::buildQuat(
		lua_tonumber(script, -4),
		lua_tonumber(script, -3),
		lua_tonumber(script, -2),
		lua_toboolean(script, -1)
	);
}

void quat_lua(lua_State* script, glm::vec3 v, bool g)
{
	lua_createtable(script, 0, 4);
	insertNumber(script, "yaw", v.x);
	insertNumber(script, "pitch", v.y);
	insertNumber(script, "roll", v.z);
	insertBoolean(script, "relative", g);
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

LUA(window_dt)
{
	auto dt = getWindow(script)->getDeltaTime();
	lua_pushnumber(script, dt);
	return 1;
}

void ae::bind::window(lua_State* script)
{
	lua_createtable(script, 0, 4);
	insertFunction(script, "close", ae_window_close);
	insertFunction(script, "clearColor", ae_window_clearColor);
	insertFunction(script, "keyPressed", ae_window_keyPressed);
	insertFunction(script, "keyJustPressed", ae_window_keyJustPressed);
	insertFunction(script, "loadUI", ae_window_loadUI);
	insertFunction(script, "size", ae_window_size);
	insertFunction(script, "uiSize", ae_window_uiSize);
	insertFunction(script, "dt", ae_window_dt);
	lua_setglobal(script, "aeWindow");
}

LUA(camera_textureUse)
{
	auto id = lua_tostring(script, -1);
	getWindow(script)->getCamera()->textureUse(id);
	return 0;
}

LUA(camera_textureSize)
{
	auto id = lua_tostring(script, -1);
	ae::Texture t = getWindow(script)->getCamera()->getTexture(id);
	vec2_lua(script, glm::vec2(t.width, t.height));
	return 1;
}

LUA(camera_shaderUse)
{
	auto id = lua_tostring(script, -1);
	getWindow(script)->getCamera()->shaderUse(id);
	return 0;
}

LUA(camera_shaderInt)
{
	auto uniform = lua_tostring(script, -2);
	auto value = lua_tointeger(script, -1);
	getWindow(script)->getCamera()->shaderInt(uniform, value);
	return 0;
}

LUA(camera_shaderVec2)
{
	auto uniform = lua_tostring(script, -2);
	auto value = lua_vec2(script);
	getWindow(script)->getCamera()->shaderVec2(uniform, value);
	return 0;
}

LUA(camera_shaderVec4)
{
	auto uniform = lua_tostring(script, -2);
	auto value = lua_vec4(script);
	getWindow(script)->getCamera()->shaderVec4(uniform, value);
	return 0;
}

LUA(camera_setModelMatrix)
{
	lua_getfield(script, -1, "pos");
	auto pos = lua_vec3(script);
	lua_getfield(script, -5, "origin");
	auto origin = lua_vec3(script);
	lua_getfield(script, -9, "scale");
	auto scale = lua_vec3(script);
	lua_getfield(script, -13, "angle");
	auto angle = lua_quat(script);
	glm::mat4 ts;
	ts = glm::translate(glm::mat4(1.0), -origin);
	ts = glm::scale(glm::mat4(1.0), scale) * ts;
	ts = glm::mat4(angle) * ts;
	ts = glm::translate(glm::mat4(1.0), pos) * ts;
	// TODO check out reference system / stack manipulation
	getWindow(script)->getCamera()->shaderSetModel(ts);
	return 0;
}

LUA(camera_drawSprite)
{
	getWindow(script)->getCamera()->drawSprite();
	return 0;
}

LUA(camera_clearCache)
{
	getWindow(script)->getCamera()->requestClearCache();
	return 0;
}

LUA(camera_createVBO)
{
	auto vbo = getWindow(script)->getCamera()->createVBO();
	lua_pushnumber(script, vbo);
	return 1;
}

LUA(camera_removeVBO)
{
	u32 vbo = lua_tonumber(script, -1);
	getWindow(script)->getCamera()->removeVBO(vbo);
	return 0;
}

LUA(camera_buildText)
{
	u32 vbo = lua_tonumber(script, -1);
	std::string str = lua_tostring(script, -2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	usize len = getWindow(script)->getCamera()->getFont()->build(str);
	lua_pushnumber(script, len);
	return 1;
}

LUA(camera_drawText)
{
	usize len = lua_tonumber(script, -1);
	u32 id = lua_tonumber(script, -2);
	auto cam = getWindow(script)->getCamera();
	cam->drawText(id, len);
	return 0;
}

void ae::bind::camera(lua_State* script)
{
	lua_createtable(script, 0, 1);
	insertFunction(script, "textureUse", ae_camera_textureUse);
	insertFunction(script, "setModelMatrix", ae_camera_setModelMatrix);
	insertFunction(script, "textureSize", ae_camera_textureSize);
	insertFunction(script, "shaderUse", ae_camera_shaderUse);
	insertFunction(script, "shaderInt", ae_camera_shaderInt);
	insertFunction(script, "shaderVec4", ae_camera_shaderVec4);
	insertFunction(script, "shaderVec2", ae_camera_shaderVec2);
	insertFunction(script, "drawSprite", ae_camera_drawSprite);
	insertFunction(script, "clearCache", ae_camera_clearCache);
	insertFunction(script, "createVBO", ae_camera_createVBO);
	insertFunction(script, "removeVBO", ae_camera_removeVBO);
	insertFunction(script, "buildText", ae_camera_buildText);
	insertFunction(script, "drawText", ae_camera_drawText);
	lua_setglobal(script, "aeCamera");
}