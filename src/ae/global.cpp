#include <ae/global.hpp>
#include <ae/types.hpp>

#include <cstdarg>
#include <nlohmann/json.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glfw/glfw3.h>

extern "C"
{
	#include <lua/lauxlib.h>
}

std::string ae::fs::readText(std::string path)
{
	auto f = fopen(path.c_str(), "r");
	if (f == nullptr) return "";
	fseek(f, 0, SEEK_END);
	auto len = ftell(f);
	fseek(f, 0, SEEK_SET);
	char* buf = (char*)malloc(len + 1);
	fread(buf, 1, len, f);
	buf[len] = 0;
	fclose(f);
	return std::string(buf);
}

json ae::fs::readJSON(std::string path)
{
	auto src = readText(path);
	if (src.empty()) src = "{}";
	return json::parse(src);
}

std::tuple<ae::usize, ae::u8*> ae::fs::readBinary(std::string path)
{
	auto f = fopen(path.c_str(), "rb");
	if (f == nullptr) return {0, nullptr};
	fseek(f, 0, SEEK_END);
	auto len = ftell(f);
	fseek(f, 0, SEEK_SET);
	u8* buf = (u8*)malloc(len);
	fread(buf, 1, len, f);
	return {
		len, buf
	};
}

void ae::str::removeAll(std::string &base, std::string part)
{
	auto pos = base.find(part);
	while (pos != std::string::npos)
	{
		base.erase(pos, part.length());
		pos = base.find(part);
	}
}

std::vector<std::string> ae::str::split(std::string base, std::string sep)
{
	std::vector<std::string> out;

	auto x = base.find(sep);
	while (x != std::string::npos)
	{
		auto p = base.substr(0, x);
		if (!p.empty()) out.push_back(p);
		base = base.substr(x + sep.length());
		x = base.find(sep);
	}

	if (!base.empty()) out.push_back(base);

	return out;
}

std::string ae::str::format(const char* style, ...)
{
	va_list args;
	char buffer[1024];
	va_start(args, style);
	i32 len = vsprintf(buffer, style, args);
	va_end(args);
	return std::string(buffer, len);
}

bool ae::script::execute(lua_State *s, const char *code)
{
	int result = luaL_dostring(s, code);
	if (result == 1)
	{
		auto err = lua_tostring(s, -1);
		printf("Failed to execute code:\n%s\n", err);
		return false;
	}
	return true;
}

bool ae::script::runFunction(lua_State *s, const char *name)
{
	if (s == nullptr) { return true; }
	lua_getglobal(s, "_executor");
	auto executor = lua_tostring(s, -1);
	auto type = lua_getglobal(s, name);
	if (type == LUA_TFUNCTION)
	{
		switch (lua_pcallk(s, 0, 0, 0,0, 0))
		{
			case LUA_OK: break;
			default:
			{
				printf("Lua \"%s\": failed to call function \"%s\":\n%s\n",
					executor, name, lua_tostring(s, -1)
				);
				return false;
			} break;
		}
	}
	else if (type != LUA_TNIL)
	{
		printf("Lua \"%s\": global \"%s\" is not a function\n",
			executor, name
		);
		return false;
	}
	return true;
}

ae::i32 ae::input::str2key(std::string key)
{
	if (key.length() == 1 && key[0] >= 65 && key[0] <= 90) { return key[0]; } // Latin
	if (key.substr(0, 3) == "Num") { return key[3]; } // Digits
	if (key[0] == 'F')
	{
		if (key.length() == 2) { return GLFW_KEY_F1 + key[1] - 49; }
		if (key.length() == 3)
		{
			return GLFW_KEY_F1 - 1 + std::stoi(key.substr(1, 2));
		} 
	}
	if (key == "Up") return GLFW_KEY_UP;
	if (key == "Down") return GLFW_KEY_DOWN;
	if (key == "Left") return GLFW_KEY_LEFT;
	if (key == "Right") return GLFW_KEY_RIGHT;
	if (key == "Escape") return GLFW_KEY_ESCAPE;
	if (key == "Enter") return GLFW_KEY_ENTER;
	if (key == "Backspace") return GLFW_KEY_BACKSPACE;
	if (key == "Space") return GLFW_KEY_SPACE;
	if (key == "LAlt") return GLFW_KEY_LEFT_ALT;
	if (key == "LShift") return GLFW_KEY_LEFT_SHIFT;
	if (key == "LCtrl") return GLFW_KEY_LEFT_CONTROL;
	if (key == "RAlt") return GLFW_KEY_RIGHT_ALT;
	if (key == "RShift") return GLFW_KEY_RIGHT_SHIFT;
	if (key == "RCtrl") return GLFW_KEY_RIGHT_CONTROL;
	if (key == "Minus") return GLFW_KEY_MINUS;
	if (key == "Equal") return GLFW_KEY_EQUAL;
	if (key == "Tab") return GLFW_KEY_TAB;
	return GLFW_KEY_LAST;
}

ae::i32 ae::input::str2btn(std::string btn)
{
	if (btn == "Left") return GLFW_MOUSE_BUTTON_LEFT;
	if (btn == "Right") return GLFW_MOUSE_BUTTON_RIGHT;
	if (btn == "Middle") return GLFW_MOUSE_BUTTON_MIDDLE;
	if (btn == "X1") return GLFW_MOUSE_BUTTON_3;
	if (btn == "X2") return GLFW_MOUSE_BUTTON_4;
	return GLFW_MOUSE_BUTTON_LAST;
}

glm::quat ae::math::buildQuat(float yaw, float pitch, float roll, bool camera)
{
	yaw = glm::radians(yaw);
	pitch = glm::radians(pitch);
	roll = glm::radians(roll);
	glm::quat q = glm::identity<glm::quat>();
	if (camera)
	{
		q = glm::rotate(q, roll, glm::vec3(0, 0, 1));
		q = glm::rotate(q, pitch, glm::vec3(1, 0, 0));
		q = glm::rotate(q, yaw, glm::vec3(0, 1, 0));
	}
	else
	{
		q = glm::rotate(q, yaw, glm::vec3(0, 1, 0));
		q = glm::rotate(q, pitch, glm::vec3(1, 0, 0));
		q = glm::rotate(q, roll, glm::vec3(0, 0, 1));
	}
	return q;
}

ae::f32 ae::math::f32(u8* data)
{
	ae::f32 f;
	memcpy(&f, data, sizeof(ae::f32));
	return f;
}

ae::u16 ae::math::u16(u8* data)
{
	ae::u16 u;
	memcpy(&u, data, sizeof(ae::u16));
	return u;
}

ae::u8* ae::math::toLE(ae::f32 data)
{
	u8* octets = new u8[4];
	memcpy(octets, &data, sizeof(data));
	return octets;
}