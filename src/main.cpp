#include "Hotkeys.h"
#include "LookupNameDefinitions.h"
#include "Options.h"
#include "UI.h"

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void MessageHandler(SKSE::MessagingInterface::Message* a_message) {
	switch (a_message->type) {
	case SKSE::MessagingInterface::kPostLoad:
		PNR::LoadNameDefinitions();
		PNR::UI::DetectRaceMenu();
		break;
	case SKSE::MessagingInterface::kDataLoaded:
		PNR::Options::Load();
		logger::info("{:*^30}", "EVENTS");
		PNR::Hotkeys::Manager::Register();
		break;
	default:
		break;
	}
}

#ifdef SKYRIM_AE
extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v;
	v.PluginVersion(Version::MAJOR);
	v.PluginName("PlayerNameRandomizer");
	v.AuthorName("sasnikol");
	v.UsesAddressLibrary();
	v.UsesNoStructs();
	v.CompatibleVersions({ SKSE::RUNTIME_SSE_LATEST });

	return v;
}();
#else
extern "C" DLLEXPORT bool SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info) {
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = "PlayerNameRandomizer";
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver <
#	ifndef SKYRIMVR
	    SKSE::RUNTIME_SSE_1_5_39
#	else
	    SKSE::RUNTIME_VR_1_4_15
#	endif
	) {
		logger::critical("Unsupported runtime version {}", ver.string());
		return false;
	}

	return true;
}
#endif

extern "C" DLLEXPORT bool SKSEPlugin_Load(const SKSE::LoadInterface* a_skse) {
	SKSE::Init(a_skse, { .logName = Version::PROJECT.data(), .logPattern = "[%H:%M:%S] %v" });
	logger::info("Game version : {}", a_skse->RuntimeVersion().string());

	SKSE::GetMessagingInterface()->RegisterListener(MessageHandler);

	return true;
}
