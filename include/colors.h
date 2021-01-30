#pragma once
#include <jsonlib.h>

namespace colors
{
	std::string version = "default";

	std::string get_color(std::string color_name)
	{
		if (version == "discord") return ""; // colors don't work in discord

		std::map<std::string, std::string> colormap{
			{"red", "\x1B[31m"},
			{"yellow", "\x1B[33m"},
			{"reset", "\x1B[0m"}
		};

		return colormap[color_name];
	}
}