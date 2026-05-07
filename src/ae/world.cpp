#include <ae/bind.hpp>
#include <ae/world.hpp>
#include <ae/global.hpp>
#include <ae/window.hpp>

#include <stdio.h>

using namespace ae::world;

void World::init()
{
	if (!this->ents.empty())
	{
		for (auto e: this->ents)
		{
			delete e.second;
		}
		this->ents.clear();
	}
	if (!this->skeletons.empty())
	{
		for (auto s: this->skeletons)
		{
			delete s.second;
		}
		this->skeletons.clear();
	}

	if (this->state != nullptr)
	{
		lua_close(this->state);
		this->state = nullptr;
	}
	this->state = luaL_newstate();
	ae::bind::setup(this->state, this->window, "world");
	ae::bind::window(this->state);
	ae::bind::camera(this->state);
	ae::bind::world(this->state);
	ae::bind::network(this->state);
	printf("Initialized World\n");
	this->ready = false;
}

World::World(ae::Window* win)
{
	this->window = win;
	this->state = nullptr;
	this->init();
}

World::~World()
{
	printf("Destroying World\n");
	for (auto e: this->ents)
	{
		delete e.second;
	}
	this->ents.clear();
	lua_close(this->state);
}

Entity* World::getEntity(const char* name)
{
	auto t = this->ents.find(name);
	if (t == this->ents.end()) return nullptr;
	return t->second;
}

bool World::load(const char* name)
{
	this->init();
	printf("Loading world \"%s\"\n", name);
	std::string src = ae::fs::readText(ae::str::format(
		"res/scripts/worlds/%s.lua", name
	));
	if (src.empty())
	{
		printf("The world file \"%s\" is empty or not found\n",
			name
		);
	}
	if (!ae::script::execute(this->state, src.c_str()))
	{
		printf("Failed to load world\n");
		return false;
	}
	return true;
}

void World::update()
{
	if (!this->reload.empty())
	{
		this->load(this->reload.c_str());
		this->reload.clear();
	}

	if (this->state == nullptr) { return; }

	if (!this->ready)
	{
		this->ready = true;
		if (!ae::script::runFunction(this->state, "Init"))
		{
			lua_close(this->state);
			this->state = nullptr;
		}
		printf("Initializing entities\n");
		for (auto e: this->ents)
		{
			e.second->init();
		}
	}

	if (!ae::script::runFunction(this->state, "Update"))
	{
		lua_close(this->state);
		this->state = nullptr;
	}

	for (auto e: this->ents)
	{
		e.second->update();
	}
}

void World::render()
{
	this->window->getCamera()->useProjection(true);
	this->window->getCamera()->useView(true);
	for (auto e: this->ents)
	{
		e.second->render();
	}
}

void World::requestReload(const char* name)
{
	this->reload = name;
}

void World::spawn(const char* id, const char* name)
{
	auto e = new Entity(this->window, id, name);
	this->ents.insert({std::string(id), e});
	if (this->ready) e->init();
}

lua_State* World::getScript() { return this->state; }

void World::setSkeleton(const char* id, mesh::Skeleton* sk)
{
	this->skeletons.insert_or_assign(id, sk);
}

ae::mesh::Skeleton* World::getSkeleton(const char* id)
{
	auto t = this->skeletons.find(id);
	if (t == this->skeletons.end()) return nullptr;
	return t->second;
}

void World::destroySkeleton(const char* id)
{
	auto t = this->skeletons.find(id);
	if (t == this->skeletons.end()) return;
	delete t->second;
	this->skeletons.erase(id);
}

Entity::Entity(ae::Window* win, const char* id, const char* path):
	mesh(nullptr)
{
	this->window = win;
	this->state = luaL_newstate();
	ae::bind::setup(this->state, win, id);
	ae::bind::window(this->state);
	ae::bind::camera(this->state);
	ae::bind::world(this->state);
	ae::bind::entity(this->state);
	ae::bind::network(this->state);
	printf("Loading entity from \"%s\"\n", path);
	std::string src = ae::fs::readText(ae::str::format(
		"res/scripts/ents/%s.lua", path
	));
	if (src.empty())
	{
		printf("The entity file \"%s\" is empty or not found (ID \"%s\")\n",
			path, id
		);
	}
	if (!ae::script::execute(this->state, src.c_str()))
	{
		printf("Failed to load entity \"%s\"\n", id);
		return;
	}

	this->mesh = mesh::Mesh(win->getCamera());
}

Entity::~Entity()
{
	lua_close(this->state);
	this->state = nullptr;
	this->mesh.destroy();
}

bool Entity::init()
{
	return ae::script::runFunction(this->state, "Init");
}

bool Entity::update()
{
	return ae::script::runFunction(this->state, "Update");
}

bool Entity::render()
{
	return ae::script::runFunction(this->state, "Draw");
}

ae::mesh::Mesh* Entity::getMesh()
{
	return &this->mesh;
}

lua_State* Entity::getScript() { return this->state; }