#include "Randomizer.h"

#include <ClibUtil/rng.hpp>

#include "LookupNameDefinitions.h"
#include "Options.h"

namespace PNR
{
	namespace Randomizer
	{
		namespace details
		{
			/// Sorts NameDefinitions by their priorities.
			///	If priorities are the same, then alphabetical order is used.
			struct definitions_priority_greater
			{
				bool operator()(const NameDefinition& lhs, const NameDefinition& rhs) const {
					if (lhs.priority > rhs.priority)
						return true;
					if (lhs.priority == rhs.priority)
						return lhs.name < rhs.name;
					return false;
				}
			};

			inline clib_util::RNG rng{};

			std::optional<NameComponents> MakeNameComponents(const RE::Actor* actor) {
				if (!actor || !actor->GetActorBase() || !actor->GetRace())
					return std::nullopt;

				const auto race = actor->GetRace();

				if (!race)
					return std::nullopt;

				if (!Options::General::mapping.contains(race->formID)) {
					logger::error("Mapping for Player's race ([0x{:X}] '{}') not found", race->formID, race->GetName());
					return std::nullopt;
				}

				if (loadedDefinitions.empty())
					return std::nullopt;

				std::vector<std::reference_wrapper<const NameDefinition>> definitions{};

				for (auto& mapping : Options::General::mapping.at(race->formID)) {
					const auto& name = mapping.nameDefinition;
					const auto  chance = actor->GetActorBase()->GetSex() == RE::SEX::kMale ? mapping.maleChance : mapping.femaleChance;
					if (loadedDefinitions.contains(name) && (chance >= 100 || rng.Generate(0, 100) < chance)) {
						const auto& definition = loadedDefinitions.at(name);
						definitions.emplace_back(definition);
					}
				}

				if (definitions.empty()) {
					return std::nullopt;
				}

				logger::info("\tRace: [0x{:X}] '{}'", race->formID, race->GetName());
				logger::info("\tSex: {}", actor->GetActorBase()->GetSex() == RE::SEX::kMale ? "Male" : "Female");

				// Sort by priorities
				std::ranges::sort(definitions, definitions_priority_greater());
				std::vector<std::string> defNames;
				std::ranges::transform(definitions.begin(), definitions.end(), std::back_inserter(defNames), [](const auto& d) { return d.get().name; });
				logger::info("\tFrom: [{}]", clib_util::string::join(defNames, ", "));

				// Assemble a name.
				NameComponents comps;
				const auto     sex = actor->GetActorBase()->GetSex();

				// Flags that determine whether a name segment was resolved and components contain final result.
				// These flags are used to handle name inheritance.
				auto resolvedFirstName = false;
				auto resolvedMiddleName = false;
				auto resolvedLastName = false;

				for (const auto& definitionRef : definitions) {
					const auto& definition = definitionRef.get();

					auto pickedFirstName = false;
					auto pickedMiddleName = false;
					auto pickedLastName = false;

					if (!resolvedFirstName) {
						pickedFirstName = definition.GetRandomFirstName(sex, comps);
						resolvedFirstName = pickedFirstName || !definition.firstName.shouldInherit;
					}

					if (!resolvedMiddleName) {
						pickedMiddleName = definition.GetRandomMiddleName(sex, comps);
						resolvedMiddleName = pickedMiddleName || !definition.middleName.shouldInherit;
					}

					if (!resolvedLastName) {
						pickedLastName = definition.GetRandomLastName(sex, comps);
						resolvedLastName = pickedLastName || !definition.lastName.shouldInherit;
					}

					const auto pickedAnyName = pickedFirstName || pickedMiddleName || pickedLastName;

					// At the moment we use first conjunction that will be picked with at least one name segment.
					// So if Name Definition only provided conjunction, it will be skipped.
					if (pickedAnyName && comps.conjunction == empty) {
						definition.GetRandomConjunction(sex, comps);
					}

					// If all segments are resolved, then we're ready :)
					if (resolvedFirstName && resolvedMiddleName && resolvedLastName)
						break;
				}
				return comps;
			}

		}

		Name Manager::MakeName() const {
			if (const auto player = RE::PlayerCharacter::GetSingleton(); player) {
				logger::info("Picking a name for Player:");
				const auto components = details::MakeNameComponents(player);
				if (components.has_value()) {
					const auto fullName = components->Assemble();
					if (fullName.has_value() && fullName != empty) {
						logger::info("\tPicked: '{}'", *fullName);
						return *fullName;
					}
				}
			}
			return "";
		}
	}
}
