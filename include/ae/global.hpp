#ifndef aeGlobal
#define aeGlobal

#include <string>
#include <json/value.h>

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

#endif