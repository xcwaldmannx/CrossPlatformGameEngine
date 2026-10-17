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

		if (const auto* buffer = std::get_if<VkDescriptorBufferInfo>(&w.mInfo))
		{
			ws.pBufferInfo = buffer;
		}
		else if (const auto* image = std::get_if<VkDescriptorImageInfo>(&w.mInfo))
		{
			ws.pImageInfo = image;
		}
		else if (const auto* buffers = std::get_if<std::vector<VkDescriptorBufferInfo>>(&w.mInfo))
		{
			if (!buffers->empty())
			{
				ws.pBufferInfo = buffers->data();
				ws.descriptorCount = static_cast<uint32_t>(buffers->size());
			}
		}
		else if (const auto* images = std::get_if<std::vector<VkDescriptorImageInfo>>(&w.mInfo))
		{
			if (!images->empty())
			{
				ws.pImageInfo = images->data();
				ws.descriptorCount = static_cast<uint32_t>(images->size());
			}
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
	const VkDeviceSize range) const
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
	VkImageView imageView) const
{
	VkDescriptorImageInfo info{};
	info.imageView = imageView;
	info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
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

void DescriptorSet::updateTextures(
	VkDevice device,
	const uint32_t binding,
	const VkDescriptorType type,
	const std::vector<VkImageView>& imageViews) const
{
	if (imageViews.empty()) return;

	std::vector<VkDescriptorImageInfo> infos;
	infos.reserve(imageViews.size());

	for (const auto& view : imageViews)
	{
		VkDescriptorImageInfo info{};
		info.imageView = view;
		info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		info.sampler = VK_NULL_HANDLE;

		infos.push_back(info);
	}

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = mHandle;
	write.dstBinding = binding;
	write.descriptorCount = static_cast<uint32_t>(infos.size());
	write.descriptorType = type;
	write.pImageInfo = infos.data();

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
