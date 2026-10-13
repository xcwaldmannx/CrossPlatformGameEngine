#pragma once

#include "../Configuration.h"

#include <string>

#include <nlohmann/json.hpp>

namespace mass
{

	class Deserializer
	{
	public:
		ModelLayout deserialize(const Configuration& config, nlohmann::json& json);
	};

}
