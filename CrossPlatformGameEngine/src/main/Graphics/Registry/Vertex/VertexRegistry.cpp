#include "VertexRegistry.h"

#include <stdexcept>

using namespace ascen;

VertexRegistry::VertexRegistry() {}

void VertexRegistry::registerVertex(VertexEntry entry)
{
	if (isRegistered(entry.mName))
	{
		throw std::runtime_error("Vertex name already registered!");
	}

	mRegisteredNames.push_back(entry.mName);
	mEntries.emplace_back(std::move(entry));
}

void VertexRegistry::reconstruct()
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

bool VertexRegistry::isRegistered(const std::string& name) const
{
	for (const auto& registeredName : mRegisteredNames)
	{
		if (name == registeredName)
		{
			return true;
		}
	}

	return false;
}

bool VertexRegistry::exists(const std::string& name) const
{
	return (mVertices.find(name) != mVertices.end());
}
