#pragma once

#include "Configuration.h"

#include <string>

namespace mass
{

	void serialize(
		const Configuration& config,
		const std::string& inputFilepath,
		const std::string& outputFilepath);

	ModelLayout deserialize(
		const Configuration& config,
		const std::string& inputFilepath);

}