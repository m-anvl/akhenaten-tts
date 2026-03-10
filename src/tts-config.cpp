#include "tts-config.hpp"

#include <format>
#include <iostream>

bool TTSConfig::parseConfigScript(const fs::path &configFile)
{
	if (!fs::exists(configFile)) {
		std::cout << std::format("The specified configuration file does not exist: \"{}\"\n",
								 configFile.string());
		return false;
	}

	lua["config"] = lua.runFile(configFile.string());
	if (!lua["config"].valid()) {
		std::cout << std::format("Lua: Unable to load specified configuration file: \"{}\"\n",
								 configFile.string());
		return false;
	}

	bool result{true};

	const std::string espeakDataStr = lua["config"]["espeakData"];
	espeakData = fs::absolute(espeakDataStr).lexically_normal();

	if (!fs::exists(espeakData)) {
		std::cout << std::format("The specified espeak-ng data folder does not exist: \"{}\"\n",
								 espeakData.string());
		result = false;
	}

	const std::string cacheStr = lua["config"]["cache"];
	cache = fs::absolute(cacheStr).lexically_normal();

	if (!lua["config"]["figures"].valid()) {
		std::cout << std::format("Figures definitions are missed in the config script.\n");
		result = false;
	}
	if (!lua["config"]["voices"].valid()) {
		std::cout << std::format("Voices definitions are missed in the config script.\n");
		result = false;
	}
	loaded = result;
	return result;
}