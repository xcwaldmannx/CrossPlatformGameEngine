#pragma once

#include "../Registry_I.h"
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

	class VertexRegistry : public Registry_I
	{
	public:
		VertexRegistry();

		void registerVertex(VertexEntry entry);

		void reconstruct() override;

		void cleanup();

	private:
		bool isRegistered(const std::string& name) const;
		bool exists(const std::string& name) const;

	private:
		std::vector<std::string> mRegisteredNames;

		std::vector<VertexEntry> mEntries;

		std::unordered_map<std::string, VertexPtr> mVertices;

		friend class VertexRegistryBackend;
	};

}
