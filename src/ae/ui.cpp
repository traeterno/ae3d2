#include <ae/ui.hpp>
#include <ae/window.hpp>
#include <ae/global.hpp>
#include <ae/bind.hpp>
#include <stdio.h>

using namespace ae;

UI::UI(Window* win)
{
	this->window = win;
	this->state = luaL_newstate();
	luaL_openlibs(this->state);
	lua_pushinteger(this->state, reinterpret_cast<uintptr_t>(win));
	lua_setglobal(this->state, "_winptr");
	printf("Created UI state; Loading functions\n");

	ae::bind::window(this->state);
	ae::bind::math(this->state);

	printf("Initialized UI\n");
}

UI::~UI()
{
	lua_close(this->state);
	printf("Destroyed UI\n");
}

bool UI::load(std::string id)
{
	printf("Loading UI \"%s\"\n", id.c_str());
	printf("Path is %s",
		ae::str::format("res/scripts/ui/%s/main.lua\n", id.c_str()).c_str()
	);
	std::string src = ae::fs::readText(
		ae::str::format("res/scripts/ui/%s/main.lua", id.c_str())
	);
	if (src.empty())
	{
		printf(
			"The UI file \"%s\" is empty or not found\n",
			id.c_str()
		);
		return false;
	}
	if (!ae::script::execute(this->state, src.c_str()))
	{
		printf("Failed to load UI\n");
		return false;
	}

	if (lua_getglobal(this->state, "Init") == LUA_TFUNCTION)
	{
		lua_call(this->state, 0, 0);
	}

	printf("Finished loading UI \"%s\"\n", id.c_str());
	return true;
}

void UI::requestReload(std::string id)
{
	this->reload = id;
}

void UI::update()
{
	if (!this->state) { return; }
	if (!this->reload.empty())
	{
		this->load(this->reload);
		this->reload.clear();
	}

	if (lua_getglobal(this->state, "Update") != LUA_TFUNCTION)
	{
		printf("Lua error: 'Update' is not a function\n");
		lua_close(this->state);
		this->state = nullptr;
		return;
	}

	lua_call(this->state, 0, 0);
}

void UI::render()
{
	if (!this->state) { return; }
	if (lua_getglobal(this->state, "Draw") != LUA_TFUNCTION)
	{
		printf("Lua error: 'Draw' is not a function\n");
		lua_close(this->state);
		this->state = nullptr;
		return;
	}

	lua_call(this->state, 0, 0);
}

void UI::resized()
{
	if (!this->state) { return; }
	if (lua_getglobal(this->state, "OnResized") != LUA_TFUNCTION) return;
	lua_call(this->state, 0, 0);
}