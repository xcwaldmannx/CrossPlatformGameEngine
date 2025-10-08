#include "Descriptor.h"

#include <stdexcept>

#include <vector>

using namespace ascen;

VkDescriptorPool Descriptor::createPool(
	VkDevice device,
	std::initializer_list<PoolSize> poolSizes)
{
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.begin();
	poolInfo.maxSets = 1;

	VkDescriptorPool pool;

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}

	return pool;
}

VkDescriptorSetLayoutBinding Descriptor::createBinding(
	uint32_t slot,
	VkDescriptorType type,
	uint32_t stageFlags)
{
	VkDescriptorSetLayoutBinding binding{};

	binding.binding = slot;
	binding.descriptorType = static_cast<VkDescriptorType>(type);
	binding.descriptorCount = 1;
	binding.stageFlags = static_cast<VkShaderStageFlagBits>(stageFlags);
	binding.pImmutableSamplers = nullptr;

	return binding;
}

VkDescriptorSetLayout Descriptor::createLayout(
	VkDevice device,
	std::initializer_list<VkDescriptorSetLayoutBinding> bindings)
{
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.begin();

	VkDescriptorSetLayout layout;

	if (vkCreateDescriptorSetLayout(
		device, &layoutInfo, nullptr, &layout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}

	return layout;
}

VkDescriptorSet Descriptor::createSet(
	VkDevice device,
	VkDescriptorPool pool,
	std::initializer_list<VkDescriptorSetLayout> layouts)
{
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
	allocInfo.pSetLayouts = layouts.begin();

	VkDescriptorSet descriptorSet;

	if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	return descriptorSet;
}

VkWriteDescriptorSet Descriptor::createBufferWrite(
	VkDescriptorSet set,
	VkDescriptorType type,
	VkBuffer buffer,
	VkDeviceSize offset,
	VkDeviceSize range,
	uint32_t bindingSlot)
{
	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = buffer;
	bufferInfo.offset = offset;
	bufferInfo.range = range;

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = set;
	write.dstBinding = bindingSlot;
	write.dstArrayElement = 0;
	write.descriptorType = type;
	write.descriptorCount = 1;
	write.pBufferInfo = &bufferInfo;

	return write;
}

VkWriteDescriptorSet Descriptor::createImageWrite(
	VkDescriptorSet set,
	VkDescriptorType type,
	VkImageView view,
	VkSampler sampler,
	uint32_t bindingSlot)
{
	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = view;
	imageInfo.sampler = sampler;

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = set;
	write.dstBinding = bindingSlot;
	write.dstArrayElement = 0;
	write.descriptorType = type;
	write.descriptorCount = 1;
	write.pImageInfo = &imageInfo;

	return write;
}

void Descriptor::updateSet(
	VkDevice device,
	VkDescriptorSet set,
	std::initializer_list<VkWriteDescriptorSet> writes)
{
	vkUpdateDescriptorSets(device, static_cast<uint32_t>(writes.size()), writes.begin(), 0, nullptr);
}

void Descriptor::destroy(
	VkDevice device,
	VkDescriptorSetLayout layout,
	VkDescriptorPool pool)
{
	vkDestroyDescriptorSetLayout(device, layout, nullptr);
	vkDestroyDescriptorPool(device, pool, nullptr);
}
