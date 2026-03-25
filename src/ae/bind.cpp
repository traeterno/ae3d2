#include <glad/glad.h>
#include <ae/bind.hpp>
#include <ae/window.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>
#include <ae/types.hpp>
#include <ae/global.hpp>
#include <ae/font.hpp>
#include <GLFW/glfw3.h>
#include <nlohmann/json.hpp>

using namespace ae;

#define LUA(f) int ae_##f(lua_State* script)

void ae::bind::setup(lua_State* script, Window* win, const char* executor)
{
	luaL_openlibs(script);

	lua_pushinteger(script, (uintptr_t)win);
	lua_setglobal(script, "_winptr");

	lua_pushstring(script, executor);
	lua_setglobal(script, "_executor");

	lua_getglobal(script, "package");
	lua_pushstring(script, "path");
	lua_pushstring(script, "./res/scripts/?.lua");
	lua_settable(script, -3);

	lua_settop(script, 0);
}

ae::Window* getWindow(lua_State* script)
{
	lua_getglobal(script, "_winptr");
	auto addr = lua_tointeger(script, -1);
	lua_pop(script, 1);
	return reinterpret_cast<ae::Window*>(addr);
}

ae::world::Entity* getEntity(lua_State* script)
{
	lua_getglobal(script, "_executor");
	std::string exec = lua_tostring(script, -1);
	lua_pop(script, 1);
	return getWindow(script)->getWorld()->getEntity(exec.c_str());
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
	glm::vec3 xyz(
		lua_tonumber(script, -3),
		lua_tonumber(script, -2),
		lua_tonumber(script, -1)
	);
	lua_pop(script, 4);
	return xyz;
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
	auto q = ae::math::buildQuat(
		lua_tonumber(script, -3),
		lua_tonumber(script, -2),
		lua_tonumber(script, -1),
		false
	);
	lua_pop(script, 4);
	return q;
}

void quat_lua(lua_State* script, glm::vec3 v, bool g)
{
	lua_createtable(script, 0, 4);
	insertNumber(script, "yaw", v.x);
	insertNumber(script, "pitch", v.y);
	insertNumber(script, "roll", v.z);
	insertBoolean(script, "relative", g);
}

std::pair<glm::mat3, glm::mat4> lua_ts(lua_State* script)
{
	lua_getfield(script, -1, "pos");
	auto pos = lua_vec3(script);
	lua_getfield(script, -1, "origin");
	auto origin = lua_vec3(script);
	lua_getfield(script, -1, "scale");
	auto scale = lua_vec3(script);
	lua_getfield(script, -1, "angle");
	auto angle = glm::mat3(lua_quat(script));
	glm::mat4 ts;
	ts = glm::translate(glm::mat4(1.0), -origin);
	ts = glm::scale(glm::mat4(1.0), scale) * ts;
	ts = glm::mat4(angle) * ts;
	ts = glm::translate(glm::mat4(1.0), pos) * ts;
	lua_pop(script, 1);
	return {angle, ts};
}

void transferTable(lua_State* l1, lua_State* l2)
{
	lua_createtable(l2, 0, 0);
	lua_pushnil(l1);
	while (lua_next(l1, -2))
	{
		lua_pushvalue(l1, -2);
		if (lua_isinteger(l1, -1))
		{
			lua_pushinteger(l2, lua_tointeger(l1, -1));
		}
		else lua_pushstring(l2, lua_tostring(l1, -1));
		lua_pop(l1, 1);

		if (lua_isstring(l1, -1))
		{
			lua_pushstring(l2, lua_tostring(l1, -1));
		}
		else if (lua_isinteger(l1, -1))
		{
			lua_pushinteger(l2, lua_tointeger(l1, -1));
		}
		else if (lua_isnumber(l1, -1))
		{
			lua_pushnumber(l2, lua_tonumber(l1, -1));
		}
		else if (lua_isboolean(l1, -1))
		{
			lua_pushboolean(l2, lua_toboolean(l1, -1));
		}
		else if (lua_istable(l1, -1))
		{
			transferTable(l1, l2);
		}

		lua_settable(l2, -3);
		lua_pop(l1, 1);
	}
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

LUA(window_isFocused)
{
	lua_pushboolean(script, glfwGetWindowAttrib(
		getWindow(script)->getGLFW(),
		GLFW_FOCUSED
	));
	return 1;
}

LUA(window_scroll)
{
	auto w = getWindow(script);
	vec2_lua(script, w->scroll);
	return 1;
}

LUA(window_mousePos)
{
	vec2_lua(script, getWindow(script)->mousePos());
	return 1;
}

LUA(window_mousePressed)
{
	std::string btn = lua_tostring(script, -1);
	lua_pushboolean(script, getWindow(script)->mousePressed(btn));
	return 1;
}

LUA(window_mouseJustPressed)
{
	std::string btn = lua_tostring(script, -1);
	auto ev = getWindow(script)->mouse;
	lua_pushboolean(script, ae::input::str2btn(btn) == ev.btn && ev.action == GLFW_PRESS);
	return 1;
}

LUA(window_textInput)
{
	lua_pushinteger(script, getWindow(script)->codepoint);
	// auto c = getWindow(script)->codepoint;
	// std::string out;
	// if (c < 192) { out.push_back(c); }
	// else
	// {
	// 	out.push_back(192 | (c >> 6));
	// 	out.push_back(128 | (c & 63));
	// }
	// lua_pushstring(script, out.c_str());
	return 1;
}

void ae::bind::window(lua_State* script)
{
	lua_createtable(script, 0, 13);
	insertFunction(script, "close", ae_window_close);
	insertFunction(script, "clearColor", ae_window_clearColor);
	insertFunction(script, "keyPressed", ae_window_keyPressed);
	insertFunction(script, "keyJustPressed", ae_window_keyJustPressed);
	insertFunction(script, "loadUI", ae_window_loadUI);
	insertFunction(script, "size", ae_window_size);
	insertFunction(script, "uiSize", ae_window_uiSize);
	insertFunction(script, "dt", ae_window_dt);
	insertFunction(script, "isFocused", ae_window_isFocused);
	insertFunction(script, "scroll", ae_window_scroll);
	insertFunction(script, "mousePos", ae_window_mousePos);
	insertFunction(script, "mousePressed", ae_window_mousePressed);
	insertFunction(script, "mouseJustPressed", ae_window_mouseJustPressed);
	insertFunction(script, "textInput", ae_window_textInput);
	lua_setglobal(script, "aeWindow");
}

LUA(camera_textureUse)
{
	auto id = lua_tostring(script, -1);
	u8 index = lua_tointeger(script, -2);
	getWindow(script)->getCamera()->textureUse(index, id);
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

LUA(camera_shaderFloat)
{
	auto uniform = lua_tostring(script, -2);
	auto value = lua_tonumber(script, -1);
	getWindow(script)->getCamera()->shaderFloat(uniform, value);
	return 0;
}

LUA(camera_shaderVec2)
{
	auto uniform = lua_tostring(script, -2);
	auto value = lua_vec2(script);
	getWindow(script)->getCamera()->shaderVec2(uniform, value);
	return 0;
}

LUA(camera_shaderVec3)
{
	auto uniform = lua_tostring(script, -2);
	auto value = lua_vec3(script);
	getWindow(script)->getCamera()->shaderVec3(uniform, value);
	return 0;
}

LUA(camera_shaderVec4)
{
	auto uniform = lua_tostring(script, -2);
	auto value = lua_vec4(script);
	getWindow(script)->getCamera()->shaderVec4(uniform, value);
	return 0;
}

LUA(camera_drawSprite)
{
	auto [rot, ts] = lua_ts(script);
	getWindow(script)->getCamera()->drawSprite(ts);
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
	glm::vec3 data = getWindow(script)->getCamera()->getFont()->build(str);
	lua_pushnumber(script, data.x);
	vec2_lua(script, glm::vec2(data.y, data.z));
	return 2;
}

LUA(camera_drawText)
{
	auto [rot, ts] = lua_ts(script);
	usize len = lua_tonumber(script, -1);
	u32 id = lua_tonumber(script, -2);
	auto cam = getWindow(script)->getCamera();
	cam->drawText(id, len, rot, ts);
	return 0;
}

LUA(camera_mouseDelta)
{
	auto w = getWindow(script);
	glm::vec2 m = w->mousePos();
	glm::vec2 c = w->getSize() * 0.5f;
	glfwSetCursorPos(w->getGLFW(), c.x, c.y);
	vec2_lua(script, m - c);
	return 1;
}

LUA(camera_buildView)
{
	lua_getfield(script, -1, "pos");
	auto pos = lua_vec3(script);
	lua_getfield(script, -1, "orientation");
	auto a = lua_vec3(script);
	lua_getfield(script, -1, "distance");
	float dist = lua_tonumber(script, -1);
	auto q = ae::math::buildQuat(a.x, a.y, a.z, true);
	glm::vec3 dir = glm::vec3(0, 0, 1) * q;
	getWindow(script)->getCamera()->lookAt(
		dist == 0 ? pos : pos - dir * dist,
		dist == 0 ? pos + dir : pos,
		glm::vec3(0, 1, 0) * q
	);
	return 0;
}

LUA(camera_lookAt)
{
	lua_getfield(script, -1, "position");
	auto pos = lua_vec3(script);
	lua_getfield(script, -1, "target");
	auto target = lua_vec3(script);
	getWindow(script)->getCamera()->lookAt(
		pos, target, glm::vec3(0, 1, 0)
	);
	return 0;
}

LUA(camera_loadGLTF)
{
	auto id = lua_tostring(script, -1);
	getWindow(script)->getCamera()->loadGLTF(id);
	return 0;
}

LUA(camera_unloadGLTF)
{
	auto id = lua_tostring(script, -1);
	getWindow(script)->getCamera()->unloadGLTF(id);
	return 0;
}

LUA(camera_drawShape)
{
	auto [rot, ts] = lua_ts(script);
	u8 type = lua_tointeger(script, -1);
	u32 count = lua_tointeger(script, -2);
	u32 vbo = lua_tointeger(script, -3);
	getWindow(script)->getCamera()->drawShape(vbo, type, count, ts);
	return 0;
}

LUA(camera_genVBO)
{
	u32 count = lua_rawlen(script, -1);
	auto data = new f32[count];
	for (u32 i = 0; i < count; i++)
	{
		lua_pushinteger(script, i + 1);
		lua_gettable(script, -2);
		data[i] = lua_tonumber(script, -1);
		lua_pop(script, 1);
	}
	lua_pop(script, 1);
	u32 id = lua_tointeger(script, -1);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER,
		count * sizeof(f32), data, GL_STATIC_DRAW
	);
	return 0;
}

LUA(camera_applyTransform)
{
	auto [rot, ts] = lua_ts(script);
	auto vec = lua_vec4(script);
	vec4_lua(script, ts * vec);
	return 1;
}

void ae::bind::camera(lua_State* script)
{
	lua_createtable(script, 0, 21);
	insertFunction(script, "textureUse", ae_camera_textureUse);
	insertFunction(script, "textureSize", ae_camera_textureSize);
	insertFunction(script, "shaderUse", ae_camera_shaderUse);
	insertFunction(script, "shaderInt", ae_camera_shaderInt);
	insertFunction(script, "shaderFloat", ae_camera_shaderFloat);
	insertFunction(script, "shaderVec2", ae_camera_shaderVec2);
	insertFunction(script, "shaderVec3", ae_camera_shaderVec3);
	insertFunction(script, "shaderVec4", ae_camera_shaderVec4);
	insertFunction(script, "drawSprite", ae_camera_drawSprite);
	insertFunction(script, "clearCache", ae_camera_clearCache);
	insertFunction(script, "createVBO", ae_camera_createVBO);
	insertFunction(script, "removeVBO", ae_camera_removeVBO);
	insertFunction(script, "buildText", ae_camera_buildText);
	insertFunction(script, "drawText", ae_camera_drawText);
	insertFunction(script, "mouseDelta", ae_camera_mouseDelta);
	insertFunction(script, "buildView", ae_camera_buildView);
	insertFunction(script, "lookAt", ae_camera_lookAt);
	insertFunction(script, "loadGLTF", ae_camera_loadGLTF);
	insertFunction(script, "unloadGLTF", ae_camera_unloadGLTF);
	insertFunction(script, "drawShape", ae_camera_drawShape);
	insertFunction(script, "genVBO", ae_camera_genVBO);
	insertFunction(script, "applyTransform", ae_camera_applyTransform);
	lua_setglobal(script, "aeCamera");
}

LUA(world_load)
{
	auto id = lua_tostring(script, -1);
	getWindow(script)->getWorld()->requestReload(id);
	return 0;
}

LUA(world_spawn)
{
	auto id = lua_tostring(script, -2);
	auto name = lua_tostring(script, -1);
	getWindow(script)->getWorld()->spawn(id, name);
	return 0;
}

LUA(world_execute)
{
	std::string fn = lua_tostring(script, -2);
	auto s = getWindow(script)->getWorld()->getScript();
	lua_getglobal(s, fn.c_str());
	transferTable(script, s);
	lua_call(s, 1, 0);
	return 0;
}

LUA(world_entExecute)
{
	std::string name = lua_tostring(script, -3);
	std::string fn = lua_tostring(script, -2);
	auto s = getWindow(script)->getWorld()->
		getEntity(name.c_str())->getScript();
	lua_getglobal(s, fn.c_str());
	transferTable(script, s);
	lua_call(s, 1, 0);
	return 0;
}

void ae::bind::world(lua_State* script)
{
	lua_createtable(script, 0, 4);
	insertFunction(script, "load", ae_world_load);
	insertFunction(script, "spawn", ae_world_spawn);
	insertFunction(script, "execute", ae_world_execute);
	insertFunction(script, "entExecute", ae_world_entExecute);
	lua_setglobal(script, "aeWorld");
}

LUA(entity_loadMesh)
{
	auto glTF = lua_tostring(script, -2);
	auto id = lua_tostring(script, -1);
	auto g = getWindow(script)->getCamera()->getGLTF(glTF);
	getEntity(script)->getMesh()->load(g, id);
	return 0;
}

LUA(entity_draw)
{
	auto [rotation, ts] = lua_ts(script);
	getEntity(script)->getMesh()->render(
		getWindow(script)->getDeltaTime(),
		rotation, ts
	);
	return 0;
}

LUA(entity_drawSkeleton)
{
	auto [rotation, ts] = lua_ts(script);
	auto sk = getEntity(script)->getMesh()->getSkeleton();
	if (sk == nullptr) return 0;
	sk->render(getWindow(script)->getCamera(), ts);
	return 0;
}

LUA(entity_startAnimation)
{
	auto sk = getEntity(script)->getMesh()->getSkeleton();
	if (sk == nullptr) return 0;
	const char* anim = lua_tostring(script, -3);
	sk->startAnimation(anim);
	return 0;
}

LUA(entity_stopAnimation)
{
	auto sk = getEntity(script)->getMesh()->getSkeleton();
	if (sk == nullptr) return 0;
	const char* anim = lua_tostring(script, -3);
	sk->stopAnimation(anim);
	return 0;
}

void ae::bind::entity(lua_State* script)
{
	lua_createtable(script, 0, 5);
	insertFunction(script, "loadMesh", ae_entity_loadMesh);
	insertFunction(script, "draw", ae_entity_draw);
	insertFunction(script, "drawSkeleton", ae_entity_drawSkeleton);
	insertFunction(script, "startAnimation", ae_entity_startAnimation);
	insertFunction(script, "stopAnimation", ae_entity_stopAnimation);
	lua_setglobal(script, "aeEntity");
}

LUA(network_connect)
{
	u16 port = lua_tointeger(script, -1); lua_pop(script, 1);
	std::string ip = lua_tostring(script, -1); lua_pop(script, 1);
	getWindow(script)->getNC()->connect(ip, port);
	return 0;
}

LUA(network_disconnect)
{
	getWindow(script)->getNC()->disconnect();
	return 0;
}

LUA(network_isReady)
{
	auto status = getWindow(script)->getNC()->isReady();
	lua_pushboolean(script, status);
	return 1;
}

LUA(network_search)
{
	getWindow(script)->getNC()->search();
	return 0;
}

LUA(network_getServerIP)
{
	auto [ip, port] = getWindow(script)->getNC()->getServerIP();
	lua_pushstring(script, ip.c_str());
	lua_pushinteger(script, port);
	return 2;
}

LUA(network_reconnect)
{
	getWindow(script)->getNC()->reconnect();
	return 0;
}

LUA(network_stopSearch)
{
	getWindow(script)->getNC()->stopSearch();
	return 0;
}

LUA(network_status)
{
	auto cs = getWindow(script)->getNC()->getConnectionStatus();
	lua_pushinteger(script, (i32)cs);
	return 1;
}

void ae::bind::network(lua_State* script)
{
	lua_createtable(script, 0, 3);
	insertFunction(script, "connect", ae_network_connect);
	insertFunction(script, "disconnect", ae_network_disconnect);
	insertFunction(script, "reconnect", ae_network_reconnect);
	insertFunction(script, "isReady", ae_network_isReady);
	insertFunction(script, "search", ae_network_search);
	insertFunction(script, "getServerIP", ae_network_getServerIP);
	insertFunction(script, "stopSearch", ae_network_stopSearch);
	insertFunction(script, "status", ae_network_status);
	lua_setglobal(script, "aeNetwork");
}