#include "DescriptorFactory.h"

#include "Pool/DescriptorPool.h"
#include "Layout/DescriptorSetLayout.h"
#include "Set/DescriptorSet.h"

#include <stdexcept>
#include <variant>

using namespace ascen;

DescriptorFactory::DescriptorFactory(VkDevice device) : mDevice(device) {}

DescriptorPoolPtr DescriptorFactory::createPool(
	const std::vector<DescriptorPool::Size>& sizes) const
{
	DescriptorPoolPtr ptr(new DescriptorPool(mDevice, sizes));
	return ptr;
}

DescriptorSetLayoutPtr DescriptorFactory::createSetLayout(
	const std::vector<DescriptorSetLayout::Binding>& bindings) const
{
	DescriptorSetLayoutPtr ptr(new DescriptorSetLayout(mDevice, bindings));
	return ptr;
}

DescriptorSetPtr DescriptorFactory::createSet(
	const DescriptorPoolPtr& pool,
	const DescriptorSetLayoutPtr& layout,
	std::vector<DescriptorSet::Write>& writes) const
{
	DescriptorSetPtr ptr(new DescriptorSet(mDevice, pool, layout, writes));
	return ptr;
}

DescriptorSetLayout::Binding DescriptorFactory::createBinding(
	uint32_t slot,
	VkDescriptorType type,
	uint32_t stageFlags) const
{
	DescriptorSetLayout::Binding binding{};

	binding.binding = slot;
	binding.descriptorType = static_cast<VkDescriptorType>(type);
	binding.descriptorCount = 1;
	binding.stageFlags = static_cast<VkShaderStageFlagBits>(stageFlags);
	binding.pImmutableSamplers = nullptr;

	return binding;
}

DescriptorSet::Write DescriptorFactory::createBufferWrite(
	VkDescriptorType type,
	VkBuffer buffer,
	VkDeviceSize offset,
	VkDeviceSize range,
	uint32_t bindingSlot) const
{
	DescriptorSet::Write bufferWrite{};
	bufferWrite.mInfo = VkDescriptorBufferInfo{};

	VkDescriptorBufferInfo& info = std::get<VkDescriptorBufferInfo>(bufferWrite.mInfo);

	info.buffer = buffer;
	info.offset = offset;
	info.range = range;

	bufferWrite.mWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	bufferWrite.mWrite.dstBinding = bindingSlot;
	bufferWrite.mWrite.dstArrayElement = 0;
	bufferWrite.mWrite.descriptorType = type;
	bufferWrite.mWrite.descriptorCount = 1;
	//bufferWrite.mWrite.pBufferInfo = &info;

	return bufferWrite;
}

DescriptorSet::Write DescriptorFactory::createImageWrite(
	VkDescriptorType type,
	VkImageView view,
	VkSampler sampler,
	uint32_t bindingSlot) const
{
	DescriptorSet::Write imageWrite{};
	imageWrite.mInfo = VkDescriptorImageInfo{};

	VkDescriptorImageInfo& info = std::get<VkDescriptorImageInfo>(imageWrite.mInfo);

	info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	info.imageView = view;
	info.sampler = sampler;

	imageWrite.mWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	imageWrite.mWrite.dstBinding = bindingSlot;
	imageWrite.mWrite.dstArrayElement = 0;
	imageWrite.mWrite.descriptorType = type;
	imageWrite.mWrite.descriptorCount = 1;
	//imageWrite.mWrite.pImageInfo = &info;

	return imageWrite;
}
