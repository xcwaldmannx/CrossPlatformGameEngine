#pragma once

#include <initializer_list>

#include <vulkan/vulkan.h>

namespace ascen
{
	
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
			std::initializer_list<PoolSize> poolSizes);

		static VkDescriptorSetLayoutBinding createBinding(
			uint32_t slot,
			VkDescriptorType type,
			uint32_t stageFlags);

		static VkDescriptorSetLayout createLayout(
			VkDevice device,
			std::initializer_list<VkDescriptorSetLayoutBinding> bindings);
		
		static VkDescriptorSet createSet(
			VkDevice device,
			VkDescriptorPool pool,
			std::initializer_list<VkDescriptorSetLayout> layouts);

		static VkWriteDescriptorSet createBufferWrite(
			VkDescriptorSet set,
			VkDescriptorType type,
			VkBuffer buffer,
			VkDeviceSize offset,
			VkDeviceSize range,
			uint32_t bindingSlot);

		static VkWriteDescriptorSet createImageWrite(
			VkDescriptorSet set,
			VkDescriptorType type,
			VkImageView view,
			VkSampler sampler,
			uint32_t bindingSlot);

		static void updateSet(
			VkDevice device,
			VkDescriptorSet set,
			std::initializer_list<VkWriteDescriptorSet> writes);

		static void destroy(
			VkDevice device,
			VkDescriptorSetLayout layout,
			VkDescriptorPool pool);
	};

}