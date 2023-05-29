#include "Options.h"

#include "CLIBUtil/distribution.hpp"
#include "CLIBUtil/hotkeys.hpp"
#include "CLIBUtil/simpleINI.hpp"
#include "Hotkeys.h"
#include "LookupNameDefinitions.h"

namespace PNR
{
	namespace Options
	{
		bool ReadHotkey(const CSimpleIniA& ini, const char* name, std::string_view& pattern, clib_util::hotkeys::KeyCombination& hotkey) {
			if (const auto rawPattern = ini.GetValue("Hotkeys", name)) {
				if (hotkey.SetPattern(rawPattern)) {
					pattern = rawPattern;
					return true;
				}
				logger::error("Failed to set '{}' as a hotkey for {}", pattern, name);
			}
			return false;
		}

		void Map(const RE::TESRace* race, std::string_view key, std::string_view entry) {
			const std::string             str(entry);
			const auto                    rawDefinitions = clib_util::string::split(str, ","sv);
			std::vector<General::Mapping> mappings;

			for (auto& rawDefinition : rawDefinitions) {
				if (rawDefinition.empty())
					continue;

				General::Mapping mapping{};
				auto             components = clib_util::string::split(rawDefinition, "@");
				mapping.nameDefinition = components[0];
				if (!loadedDefinitions.contains(mapping.nameDefinition)) {
					logger::warn("'{} = {}' mapping will be skipped: '{}' Name Definition was not found.", key, entry, mapping.nameDefinition);
					return;
				}

				if (components.size() > 1) {
					auto& rawChances = components[1];
					auto  chances = clib_util::string::split(rawChances, "|");

					mapping.maleChance = clib_util::string::to_num<uint8_t>(chances[0]);
					if (chances.size() > 1)
						mapping.femaleChance = clib_util::string::to_num<uint8_t>(chances[1]);
					else
						mapping.femaleChance = mapping.maleChance;
				}
				if (mapping.maleChance < 100 || mapping.femaleChance < 100)
					logger::info("Registering {} (M: {}%, F: {}%) for [0x{:X}] ('{}')", mapping.nameDefinition, mapping.maleChance, mapping.femaleChance, race->formID, race->GetName());
				else
					logger::info("Registering {} for [0x{:X}] ('{}')", mapping.nameDefinition, race->formID, race->GetName());
				mappings.push_back(mapping);
			}

			General::mapping[race->formID] = mappings;
		}

		void Load() {
			logger::info("{:*^30}", "OPTIONS");
			std::filesystem::path options = R"(Data\SKSE\Plugins\PlayerNameRandomizer.ini)";
			CSimpleIniA           ini{};
			ini.SetUnicode();
			if (ini.LoadFile(options.string().c_str()) >= 0) {
				if (const auto rawMapping = ini.GetSection("Mapping"); rawMapping && !rawMapping->empty()) {
					logger::info("Mapping:");
					if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
						for (const auto& [key, entry] : *rawMapping) {
							const auto formOrEditorID = clib_util::distribution::get_record(key.pItem);

							if (std::holds_alternative<clib_util::distribution::formid_pair>(formOrEditorID)) {
								if (auto [formID, modName] = std::get<clib_util::distribution::formid_pair>(formOrEditorID); formID) {
									if (auto race = modName ? dataHandler->LookupForm<RE::TESRace>(*formID, *modName) : RE::TESForm::LookupByID<RE::TESRace>(*formID)) {
										Map(race, key.pItem, entry);
									} else {
										logger::warn("Race [0x{:X}] doesn't exist in {}", *formID, modName.value_or("the game"));
									}
								}
							} else if (std::holds_alternative<std::string>(formOrEditorID)) {
								if (auto editorID = std::get<std::string>(formOrEditorID); !editorID.empty()) {
									if (auto race = RE::TESForm::LookupByEditorID<RE::TESRace>(editorID)) {
										Map(race, key.pItem, entry);
									} else {
										logger::warn("Race {} doesn't exist", editorID);
									}
								}
							}
						}
					}
				}
				logger::info("");

				const auto manager = PNR::Hotkeys::Manager::GetSingleton();
				ReadHotkey(ini, "sGenerate", Hotkeys::generate, manager->generate);
				ReadHotkey(ini, "sReloadSettings", Hotkeys::reloadSettings, manager->reloadSettings);

			} else {
				logger::info(R"(Data\SKSE\Plugins\NPCsNamesDistributor.ini not found.)");
				logger::info("");
			}

			logger::info("Hotkeys:");
			logger::info("\tGenerate Name: {}", Hotkeys::generate);
			logger::info("\tReload Settings: {}", Hotkeys::reloadSettings);
		}
	}
}
