#pragma once

#include "../Registry_I.h"

#include "../../Core/VulkanContext.h"
#include "../Resource/ResourceRegistry.h"

#include <unordered_map>

namespace ascen
{

	enum DescriptorType
	{
		UBO = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		UBO_DYNAMIC = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
		SSBO = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		SSBO_DYNAMIC = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
		IMAGE = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
		SAMPLER = VK_DESCRIPTOR_TYPE_SAMPLER,
		IMAGE_SAMPLER = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
	};

	enum DescriptorStage
	{
		VERTEX = VK_SHADER_STAGE_VERTEX_BIT,
		PIXEL = VK_SHADER_STAGE_FRAGMENT_BIT,
		COMPUTE = VK_SHADER_STAGE_COMPUTE_BIT,
	};

	struct DescriptorSetEntry
	{
		std::string mResourceName;
		std::string mSetName;
		uint32_t mSlot;
		uint64_t mSize;
		DescriptorType mType;
		DescriptorStage mStage;
	};

	class DescriptorRegistry : public Registry_I
	{
	public:
		DescriptorRegistry(
			VulkanContext& context,
			ResourceRegistry& resourceRegistry);

		void registerDescriptor(DescriptorSetEntry entry);

		void reconstruct() override;

		void cleanup();

	private:
		bool exists(const std::string& name) const;

	private:
		const VkDevice mDevice;
		const DescriptorFactory& mDescriptorFactory;
		const ResourceRegistry& mResourceRegistry;

		DescriptorPoolPtr mDescriptorPool = nullptr;

		std::unordered_map<DescriptorType, uint32_t> mDescriptorPoolRequirements;

		std::unordered_map<std::string, std::vector<DescriptorSetEntry>> mSetEntries;

		std::unordered_map<std::string, DescriptorSetLayoutPtr> mDescriptorSetLayouts;
		std::unordered_map<std::string, DescriptorSetPtr> mDescriptorSets;

		friend class DescriptorRegistryBackend;
	};

}
