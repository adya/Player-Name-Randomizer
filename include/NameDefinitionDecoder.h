#pragma once

#include "NameDefinition.h"

namespace PNR
{
	struct NameDefinitionDecoder
	{
		/// May throw
		NameDefinition decode(const std::filesystem::path& a_path) const;
	};
}
