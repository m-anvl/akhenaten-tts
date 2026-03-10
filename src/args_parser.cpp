#include "tts.hpp"

#include <cxxopts.hpp>
#include <filesystem>
#include <format>
#include <iostream>
#include <string>
#include <ranges>

namespace fs = std::filesystem;

bool parseCmdLineArguments(int argc, char* argv[], TTSConfig &config, tts::SynthRequest &synthRequest)
{
	bool resultOk = true;

	auto options = cxxopts::Options{"akhenaten-tts", 
									"Tool to TTS akhenaten phrases"};
	options.add_options()
		("h,help", "Print usage")
		("f,figure", "Character for whom speech is synthesized.", cxxopts::value<std::string>())
		("p,phrase", "Phrase for which we synthesize speech.", cxxopts::value<std::string>())
		("l,lang", "The phrase's language.", cxxopts::value<std::string>())
		("o,output", "Output audio filename.", cxxopts::value<fs::path>())
		("c,config", "Synthesizer configuration file.", cxxopts::value<fs::path>()->default_value("tts-config.lua"));
	
	options.allow_unrecognised_options();
	const auto parsed = options.parse(argc, argv);
	
	const auto unmatched = parsed.unmatched();
	if (!unmatched.empty()) {
		auto print = [](const auto &arg) { std::cout << std::format(" \"{}\"", arg); };

		std::cout << std::format("Unrecognized command line argument(s):");
		std::ranges::for_each(unmatched, print);
		std::cout << std::format("\n{}\n", options.help());
		return false;
	}
	if (parsed.count("help") || argc == 1) {
		std::cout << std::format("{}\n", options.help());
		exit(0);
	}

	if (parsed.count("figure")) {
		synthRequest.figure = parsed["figure"].as<std::string>();
	} else {
		std::cout << std::format("Figure/character is required (--figure)\n");
		resultOk = false;
	}

	if (parsed.count("phrase")) {
		synthRequest.phrase = parsed["phrase"].as<std::string>();
	} else {
		std::cout << std::format("Text phrase to synthesize is required (--phrase)\n");
		resultOk = false;
	}

	if (parsed.count("output")) {
		synthRequest.outputFilename = fs::absolute(parsed["output"].as<fs::path>()).lexically_normal();
	} else {
		std::cout << std::format("Output filename is required (--output)\n");
		resultOk = false;
	}

	if (parsed.count("lang")) {
		synthRequest.lang = parsed["lang"].as<std::string>();
	} else {
		std::cout << std::format("Language is required (--lang)\n");
		resultOk = false;
	}

	const auto configFile = fs::absolute(parsed["config"].as<fs::path>()).lexically_normal();
	if (!config.parseConfigScript(configFile)) {
		std::cout << std::format("Unable to parse configuration file: \"{}\"\n",
								 configFile.string());
		return false;
	}
	return resultOk;
}