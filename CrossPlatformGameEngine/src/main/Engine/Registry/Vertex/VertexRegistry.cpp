#include "VertexRegistry.h"

#include "../../Vertex/Vertex.h"

#include <stdexcept>

using namespace ascen;

void VertexRegistry::registerVertex(const VertexEntry& entry)
{
	mEntries.emplace_back(entry);
}

void VertexRegistry::reconstruct(const std::unordered_map<uint64_t, RegisteredResource>& registeredResources)
{
	cleanup();

	for (auto& entry : mEntries)
	{
		mVertices[entry.mName] = std::make_shared<Vertex>(entry.mBinding, entry.mAttributes);
	}
}

void VertexRegistry::cleanup()
{
	mVertices.clear();
}
