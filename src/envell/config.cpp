#include <envell/config.hpp>
#include <nlohmann/json.hpp>

json envell::cfg::loadConfig()
{
	return ae::fs::readJSON("res/system/cfg.json");
}

void envell::cfg::saveConfig(json& v)
{
	std::string out = v.dump();
	auto f = fopen("res/system/cfg.json", "w");
	fwrite(out.c_str(), sizeof(char), out.length(), f);
	fclose(f);
}

json envell::cfg::defaultConfig()
{
	return json {
		{ "playersCount", 8 },
		{ "tickRate", 20 }
	};
}