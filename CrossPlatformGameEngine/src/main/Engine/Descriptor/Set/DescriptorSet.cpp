#include "DescriptorSet.h"

#include "../Pool/DescriptorPool.h"
#include "../Layout/DescriptorSetLayout.h"

#include <stdexcept>

using namespace ascen;

DescriptorSet::DescriptorSet(
	VkDevice device,
	const DescriptorPoolPtr& pool,
	const DescriptorSetLayoutPtr& layout,
	std::vector<Write> writes) :
	mPool(pool->handle())
{
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool->handle();
	allocInfo.descriptorSetCount = 1;

	allocInfo.pSetLayouts = &layout->handle();
	
	VkResult res = vkAllocateDescriptorSets(device, &allocInfo, &mHandle);
	if (res != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	std::vector<VkWriteDescriptorSet> vkWrites;
	vkWrites.reserve(writes.size());

	for (auto& w : writes)
	{
		VkWriteDescriptorSet ws = w.mWrite;
		ws.dstSet = mHandle;

		if (std::holds_alternative<VkDescriptorBufferInfo>(w.mInfo))
		{
			ws.pBufferInfo = &std::get<VkDescriptorBufferInfo>(w.mInfo);
			ws.pImageInfo = nullptr;
		}
		else
		{
			ws.pImageInfo = &std::get<VkDescriptorImageInfo>(w.mInfo);
			ws.pBufferInfo = nullptr;
		}

		vkWrites.push_back(ws);
	}

	vkUpdateDescriptorSets(device, static_cast<uint32_t>(vkWrites.size()), vkWrites.data(), 0, nullptr);
}

void DescriptorSet::updateBuffer(
	VkDevice device,
	const uint32_t binding,
	const VkDescriptorType type,
	VkBuffer buffer,
	const VkDeviceSize range)
{
	const VkDescriptorBufferInfo info
	{
		.buffer = buffer,
		.offset = 0,
		.range = range
	};

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = mHandle;
	write.dstBinding = binding;
	write.descriptorCount = 1;
	write.descriptorType = type;
	write.pBufferInfo = &info;

	vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
}

void DescriptorSet::updateTexture(
	VkDevice device,
	const uint32_t binding,
	const VkDescriptorType type,
	VkImageView imageView)
{
	VkDescriptorImageInfo info{};
	info.imageView = imageView;
	info.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	info.sampler = VK_NULL_HANDLE;

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = mHandle;
	write.dstBinding = binding;
	write.descriptorCount = 1;
	write.descriptorType = type;
	write.pImageInfo = &info;

	vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
}

void DescriptorSet::destroy(VkDevice device)
{
	if (mHandle == VK_NULL_HANDLE)
		return;

	if (vkFreeDescriptorSets(device, mPool, 1, &mHandle) != VK_SUCCESS)
		throw std::runtime_error("failed to free descriptor set");

	mHandle = VK_NULL_HANDLE;
}
