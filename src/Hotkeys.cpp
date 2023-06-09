#include "Hotkeys.h"

#include "LookupNameDefinitions.h"
#include "Randomizer.h"
#include "UI.h"

namespace PNR
{
	// Events
	namespace Hotkeys
	{
		void Manager::Register() {
			if (const auto scripts = RE::BSInputDeviceManager::GetSingleton()) {
				scripts->AddEventSink<RE::InputEvent*>(GetSingleton());
				logger::info("Registered for {}", typeid(RE::InputEvent).name());
			}
		}

		void Manager::GenerateTrigger(const KeyCombination*) {
			if (const auto name = Randomizer::Manager::GetSingleton()->MakeName(); name != empty) {
				if (const auto raceMenu = RE::UI::GetSingleton()->GetMovieView("RaceSex Menu").get(); raceMenu) {
					// Increase the limit of chars to allow longer names.
					raceMenu->SetVariable(UI::maxChars.data(), 50);
					raceMenu->SetVariable(UI::text.data(), name.data(), RE::GFxMovie::SetVarType::kNormal);
				} else {
					logger::error("Failed to set a name.");
				}
			} else {
				logger::error("Failed to pick a name.");
			}
		}

		void Manager::ReloadSettingsTrigger(const KeyCombination* keys) {
			logger::info("Reloading settings..");
			Options::Load();
		}

		RE::BSEventNotifyControl Manager::ProcessEvent(RE::InputEvent* const* a_event,
		                                               RE::BSTEventSource<RE::InputEvent*>*) {
			using EventType = RE::INPUT_EVENT_TYPE;
			using Result = RE::BSEventNotifyControl;

			if (!a_event)
				return Result::kContinue;

			const auto ui = RE::UI::GetSingleton();

			if (!ui->IsMenuOpen("RaceSex Menu")) {
				return Result::kContinue;
			}

			if (generate.Process(a_event))
				return Result::kStop;

			reloadSettings.Process(a_event);

			return Result::kContinue;
		}

		Manager::Manager() {
			if (!generate.SetPattern(Options::Hotkeys::generate))
				logger::error("Failed to set Key Combination for generate", Options::Hotkeys::generate);
			if (!reloadSettings.SetPattern(Options::Hotkeys::reloadSettings))
				logger::error("Failed to set Key Combination '{}' for reloadSettings", Options::Hotkeys::reloadSettings);
		}
	}

}
