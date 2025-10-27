#include "Descriptor.h"

#include <stdexcept>

#include <vector>

using namespace ascen;

VkDescriptorPool Descriptor::createPool(
	VkDevice device,
	std::vector<PoolSize>& poolSizes)
{
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
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
	std::vector<VkDescriptorSetLayoutBinding>& bindings)
{
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

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
	std::vector<VkDescriptorSetLayout>& layouts)
{
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
	allocInfo.pSetLayouts = layouts.data();

	VkDescriptorSet descriptorSet;

	if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	return descriptorSet;
}

DescriptorBufferWrite Descriptor::createBufferWrite(
	VkDescriptorSet set,
	VkDescriptorType type,
	VkBuffer buffer,
	VkDeviceSize offset,
	VkDeviceSize range,
	uint32_t bindingSlot)
{
	DescriptorBufferWrite bufferWrite{};

	bufferWrite.mInfo.buffer = buffer;
	bufferWrite.mInfo.offset = offset;
	bufferWrite.mInfo.range = range;

	bufferWrite.mWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	bufferWrite.mWrite.dstSet = set;
	bufferWrite.mWrite.dstBinding = bindingSlot;
	bufferWrite.mWrite.dstArrayElement = 0;
	bufferWrite.mWrite.descriptorType = type;
	bufferWrite.mWrite.descriptorCount = 1;
	bufferWrite.mWrite.pBufferInfo = &bufferWrite.mInfo;

	return bufferWrite;
}

DescriptorImageWrite Descriptor::createImageWrite(
	VkDescriptorSet set,
	VkDescriptorType type,
	VkImageView view,
	VkSampler sampler,
	uint32_t bindingSlot)
{
	DescriptorImageWrite imageWrite{};

	imageWrite.mInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageWrite.mInfo.imageView = view;
	imageWrite.mInfo.sampler = sampler;

	imageWrite.mWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	imageWrite.mWrite.dstSet = set;
	imageWrite.mWrite.dstBinding = bindingSlot;
	imageWrite.mWrite.dstArrayElement = 0;
	imageWrite.mWrite.descriptorType = type;
	imageWrite.mWrite.descriptorCount = 1;
	imageWrite.mWrite.pImageInfo = &imageWrite.mInfo;

	return imageWrite;
}

void Descriptor::updateSet(
	VkDevice device,
	std::vector<DescriptorWrite>& writes)
{
	std::vector<VkWriteDescriptorSet> vkWrites;
	vkWrites.reserve(writes.size());

	for (auto& write : writes)
	{
		vkWrites.push_back(write.mWrite);
	}

	vkUpdateDescriptorSets(device, static_cast<uint32_t>(vkWrites.size()), vkWrites.data(), 0, nullptr);
}

void Descriptor::destroy(
	VkDevice device,
	VkDescriptorSetLayout layout,
	VkDescriptorPool pool)
{
	vkDestroyDescriptorSetLayout(device, layout, nullptr);
	vkDestroyDescriptorPool(device, pool, nullptr);
}
