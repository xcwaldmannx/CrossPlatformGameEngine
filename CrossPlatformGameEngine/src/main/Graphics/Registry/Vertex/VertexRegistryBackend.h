#pragma once

#include "VertexRegistry.h"

namespace ascen
{

	class VertexRegistryBackend
	{
	public:
		static const VertexPtr& getVertex(VertexRegistry& registry, const std::string& name);
	};

}