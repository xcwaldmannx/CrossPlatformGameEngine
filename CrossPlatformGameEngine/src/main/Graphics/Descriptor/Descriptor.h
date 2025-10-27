#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{
	struct DescriptorWrite
	{
		VkWriteDescriptorSet mWrite{};
	};

	struct DescriptorBufferWrite : public DescriptorWrite
	{
		VkDescriptorBufferInfo mInfo{};
	};

	struct DescriptorImageWrite : public DescriptorWrite
	{
		VkDescriptorImageInfo mInfo{};
	};
	
	class Descriptor
	{
	public:
		class PoolSize : public VkDescriptorPoolSize
		{
		public:
			PoolSize(VkDescriptorType type, uint32_t descriptorCount)
			{
				this->type = type;
				this->descriptorCount = descriptorCount;
			}
		};

	public:
		static VkDescriptorPool createPool(
			VkDevice device,
			std::vector<PoolSize>& poolSizes);

		static VkDescriptorSetLayoutBinding createBinding(
			uint32_t slot,
			VkDescriptorType type,
			uint32_t stageFlags);

		static VkDescriptorSetLayout createLayout(
			VkDevice device,
			std::vector<VkDescriptorSetLayoutBinding>& bindings);
		
		static VkDescriptorSet createSet(
			VkDevice device,
			VkDescriptorPool pool,
			std::vector<VkDescriptorSetLayout>& layouts);

		static DescriptorBufferWrite createBufferWrite(
			VkDescriptorSet set,
			VkDescriptorType type,
			VkBuffer buffer,
			VkDeviceSize offset,
			VkDeviceSize range,
			uint32_t bindingSlot);

		static DescriptorImageWrite createImageWrite(
			VkDescriptorSet set,
			VkDescriptorType type,
			VkImageView view,
			VkSampler sampler,
			uint32_t bindingSlot);

		static void updateSet(
			VkDevice device,
			std::vector<DescriptorWrite>& writes);

		static void destroy(
			VkDevice device,
			VkDescriptorSetLayout layout,
			VkDescriptorPool pool);
	};

}