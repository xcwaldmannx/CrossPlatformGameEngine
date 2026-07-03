#pragma once

#include "../Registry.h"
#include "../../Core/Types.h"

#include <string>
#include <unordered_map>
#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	using VertexBinding = VkVertexInputBindingDescription;
	using VertexAttribute = VkVertexInputAttributeDescription;

	struct VertexEntry
	{
		std::string mName;
		VertexBinding mBinding;
		std::vector<VertexAttribute> mAttributes;
	};

	class VertexRegistry : public Registry
	{
	public:
		void registerVertex(const VertexEntry& entry);

		void reconstruct(const std::unordered_map<uint64_t, RegisteredResource>& registeredResources);

		void cleanup();

	private:
		std::vector<VertexEntry> mEntries;
		std::unordered_map<std::string, VertexPtr> mVertices;

		friend class VertexRegistryBackend;
	};

}
