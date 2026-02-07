#include <ae/ui.hpp>
#include <ae/window.hpp>
#include <ae/global.hpp>
#include <ae/bind.hpp>
#include <stdio.h>

using namespace ae;

UI::UI(Window* win)
{
	this->window = win;
	this->state = nullptr;
	this->init();
}

void UI::init()
{
	if (this->state) { lua_close(this->state); this->state = nullptr; }
	this->state = luaL_newstate();
	luaL_openlibs(this->state);
	lua_pushinteger(this->state, reinterpret_cast<uintptr_t>(this->window));
	lua_setglobal(this->state, "_winptr");
	lua_pushstring(this->state, "ui");
	lua_setglobal(this->state, "_executor");
	printf("Created UI state; Loading functions\n");

	ae::bind::window(this->state);
	ae::bind::math(this->state);

	printf("Initialized UI\n");
}

UI::~UI()
{
	if (this->state != nullptr)
	{
		lua_close(this->state);
		this->state = nullptr;
	}
	printf("Destroyed UI\n");
}

bool UI::load(std::string id)
{
	this->init();
	printf("Loading UI \"%s\"\n", id.c_str());
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

	if (!ae::script::runFunction(this->state, "Init"))
	{
		lua_close(this->state);
		this->state = nullptr;
		return false;
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
	if (!this->reload.empty())
	{
		this->load(this->reload);
		this->reload.clear();
	}

	if (!ae::script::runFunction(this->state, "Update"))
	{
		lua_close(this->state);
		this->state = nullptr;
	}
}

void UI::render()
{
	if (!ae::script::runFunction(this->state, "Draw"))
	{
		lua_close(this->state);
		this->state = nullptr;
	}
}

void UI::resized()
{
	if (!ae::script::runFunction(this->state, "OnResized"))
	{
		lua_close(this->state);
		this->state = nullptr;
	}
}