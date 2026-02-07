#include <ae/global.hpp>
#include <ae/types.hpp>
#include <cstdarg>
#include <json/json.h>
#include <json/value.h>

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
	char* buf = (char*)malloc(len);
	fread(buf, 1, len, f);
	fclose(f);
	return std::string(buf, len);
}

Json::Value ae::fs::readJSON(std::string path)
{
	auto src = readText(path);
	Json::Value root;
	Json::CharReaderBuilder().newCharReader()->parse(
		src.begin().base(), src.end().base(),
		&root, nullptr
	);
	return root;
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