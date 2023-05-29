#pragma once
#include "NameDefinition.h"

namespace PNR
{
	namespace Randomizer
	{

		class Manager
		{
		public:
			static Manager* GetSingleton() {
				static Manager singleton;
				return &singleton;
			}

			Name MakeName() const;
		
		private:
			// Singleton stuff :)
			Manager() = default;
			Manager(const Manager&) = delete;
			Manager(Manager&&) = delete;

			Manager& operator=(const Manager&) = delete;
			Manager& operator=(Manager&&) = delete;
		};
	}
}
