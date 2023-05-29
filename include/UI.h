#pragma once
namespace PNR
{
	namespace UI
	{
		namespace Vanilla
		{
			inline std::string_view maxChars = "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.NameEntryInstance.TextInputInstance.maxChars"sv;
			inline std::string_view text = "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.NameEntryInstance.TextInputInstance.text"sv;
		}

		namespace RaceMenu
		{
			inline std::string_view maxChars = "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.textEntry.TextInputInstance.maxChars"sv;
			inline std::string_view text = "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.textEntry.TextInputInstance.text"sv;
		}

		inline std::string_view text = Vanilla::text;
		inline std::string_view maxChars = Vanilla::maxChars;

		inline void DetectRaceMenu() {
			logger::info("{:*^30}", "RaceMenu");
			if (GetModuleHandle(L"skee64") != nullptr) {
				logger::info("RaceMenu detected!");
				text = RaceMenu::text;
				maxChars = RaceMenu::maxChars;
			} else {
				logger::info("RaceMenu not detected!");
				text = Vanilla::text;
				maxChars = Vanilla::maxChars;
			}
		}
	}
}
