#ifndef aeConfig
#define aeConfig

#include <ae/global.hpp>

namespace envell::cfg
{

json loadConfig();
void saveConfig(json& v);
json defaultConfig();

}

#endif