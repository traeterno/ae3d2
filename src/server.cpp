#include <ae/sync.hpp>
#include <ae/global.hpp>
#include <nlohmann/json.hpp>

#include <ctime>
#include <thread>

#include <ae/network.hpp>
#include <envell/players.hpp>
#include <envell/config.hpp>

int main(int argc, char* argv[])
{
	srand(time(0));
	ae::net::init();
	auto cfg = envell::cfg::loadConfig();
	if (cfg.empty())
	{
		printf("Configuration file was not found. A new one is created.\n");
		cfg = envell::cfg::defaultConfig();
		envell::cfg::saveConfig(cfg);
	}
	if (argc > 1)
	{
		if (strcmp(argv[1], "newConfig") == 0)
		{
			printf("Resetting the configuration.\n");
			cfg = envell::cfg::defaultConfig();
			envell::cfg::saveConfig(cfg);
		}
	}

	auto playersSocket = ae::sync::buildPair();
	auto players = playersSocket[0];
	auto playersThread = std::thread(
		envell::players::main,
		playersSocket[1]
	);

	ae::sync::send(players, {
		{ "setup", cfg }
	});

	auto tickRate = std::chrono::milliseconds(
		1000 / (ae::u16)cfg["tickRate"]
	);

	bool running = true;
	while (running)
	{
		std::this_thread::sleep_for(tickRate);
		// TODO restart thread if it died
	}

	playersThread.join();
	ae::net::shutdown();
	delete[] playersSocket;
	return 0;
}