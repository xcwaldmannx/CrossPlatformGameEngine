#pragma once

#include "Model/Model.h"
#include "Configuration.h"

#include <string>

namespace mass
{

	class Serializer
	{
	public:
		void serialize(
			const Configuration& config,
			Model& model,
			const std::string& outputFilepath);
	};

}
