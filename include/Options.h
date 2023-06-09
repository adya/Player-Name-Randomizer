#pragma once

namespace PNR
{
	namespace Options
	{
		namespace General
		{
			struct Mapping
			{
				std::string  nameDefinition;
				std::uint8_t maleChance{ 100 };
				std::uint8_t femaleChance{ 100 };
			};

			inline std::unordered_map<RE::FormID, std::vector<Mapping>> mapping{};
		}

		namespace Hotkeys
		{
			inline std::string generate = "Insert";
			inline std::string reloadSettings = "RCtrl+L";
		}

		void Load();
	}
}
