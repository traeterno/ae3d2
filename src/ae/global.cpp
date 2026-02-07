#include <ae/global.hpp>
#include <ae/types.hpp>
#include <cstdarg>
#include <json/json.h>
#include <json/value.h>

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