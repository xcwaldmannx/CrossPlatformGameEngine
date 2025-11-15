#pragma once

#include "../Types.h"

#include "Pool/DescriptorPool.h"
#include "Layout/DescriptorSetLayout.h"
#include "Set/DescriptorSet.h"

#include <cstdint>

#include <vulkan/vulkan.h>

namespace ascen
{

	class DescriptorFactory
	{
	public:
		DescriptorFactory(VkDevice device);

		DescriptorPoolPtr createPool(
			const std::vector<DescriptorPool::Size>& sizes) const;

		DescriptorSetLayoutPtr createLayout(
			const std::vector<DescriptorSetLayout::Binding>& bindings) const;

		DescriptorSetPtr createSet(
			const DescriptorPoolPtr& pool,
			const DescriptorSetLayoutPtr& layout,
			std::vector<DescriptorSet::Write>& writes) const;

		DescriptorSetLayout::Binding createBinding(
			uint32_t slot,
			VkDescriptorType type,
			uint32_t stageFlags) const;

		DescriptorSet::Write createBufferWrite(
			VkDescriptorType type,
			VkBuffer buffer,
			VkDeviceSize offset,
			VkDeviceSize range,
			uint32_t bindingSlot) const;

		DescriptorSet::Write createImageWrite(
			VkDescriptorType type,
			VkImageView view,
			VkSampler sampler,
			uint32_t bindingSlot) const;

	private:
		VkDevice mDevice = VK_NULL_HANDLE;
	};

}
