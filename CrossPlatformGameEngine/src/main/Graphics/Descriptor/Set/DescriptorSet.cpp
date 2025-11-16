#include "DescriptorSet.h"

#include <stdexcept>

using namespace ascen;

DescriptorSet::DescriptorSet(
	const DescriptorPoolPtr& pool,
	const DescriptorSetLayoutPtr& layout,
	std::vector<Write> writes)
{
	mAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	mAllocInfo.descriptorPool = pool->handle();
	mAllocInfo.descriptorSetCount = 1;
	mAllocInfo.pSetLayouts = &layout->handle();
}

void DescriptorSet::create(VkDevice device)
{
	if (vkAllocateDescriptorSets(device, &mAllocInfo, &mHandle) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	std::vector<VkWriteDescriptorSet> vkWrites;
	vkWrites.reserve(mWrites.size());

	for (auto& w : mWrites)
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

void DescriptorSet::destroy(VkDevice device)
{
	// not used, destroyed by its pool
}
