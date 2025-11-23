#include "VertexRegistryBackend.h"

using namespace ascen;

const VertexPtr& VertexRegistryBackend::getVertex(const VertexRegistry& registry, const std::string& name)
{
	if (registry.exists(name))
	{
		return registry.mVertices.at(name);
	}
}
