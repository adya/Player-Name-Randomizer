#pragma once
#include "CLIBUtil/hotkeys.hpp"
#include "Options.h"

namespace PNR
{
	namespace Hotkeys
	{
		using namespace clib_util::hotkeys;
		class Manager : public RE::BSTEventSink<RE::InputEvent*>
		{
		public:
			static Manager* GetSingleton() {
				static Manager singleton;
				return &singleton;
			}

			static void Register();

			static void GenerateTrigger(const KeyCombination*);
			static void ReloadSettingsTrigger(const KeyCombination*);
		
			KeyCombination generate{ GenerateTrigger };
			KeyCombination reloadSettings{ ReloadSettingsTrigger };
		protected:
			RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) override;

		private:
			// Singleton stuff :)
			Manager();
			Manager(const Manager&) = delete;
			Manager(Manager&&) = delete;

			~Manager() override = default;

			Manager& operator=(const Manager&) = delete;
			Manager& operator=(Manager&&) = delete;
		};
	}
}
