#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace vulkan {

	enum DescriptorType {
		UNIFORM_BUFFER = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	};

	enum DescriptorStageFlags {
		VERTEX_SHADER = VK_SHADER_STAGE_VERTEX_BIT,
		PIXEL_SHADER = VK_SHADER_STAGE_FRAGMENT_BIT,
		COMPUTE_SHADER = VK_SHADER_STAGE_COMPUTE_BIT,
	};

	typedef uint32_t DescriptorSet;

	class DescriptorSetLayout {
	public:
		void init(VkDevice logicalDevice);
		DescriptorSet createDescriptorSet(uint32_t slot, DescriptorType type, DescriptorStageFlags flags);
		void destroy();

		const VkDescriptorSetLayout& get() const;

	private:
		VkDevice mLogicalDevice;
		VkDescriptorSetLayout mDescriptorSetLayout;
		std::vector<VkDescriptorSetLayoutBinding> mDescriptorSets;
	};

}