#include "DescriptorSet.h"

#include <stdexcept>

using namespace ascen;

DescriptorSet::DescriptorSet(
	const DescriptorPoolPtr& pool,
	const DescriptorSetLayoutPtr& layout,
	const std::vector<Write>& writes) : mWrites(writes)
{
	VkDescriptorSetLayout& ref = layout->handle();

	mAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	mAllocInfo.descriptorPool = pool->handle();
	mAllocInfo.descriptorSetCount = 1;
	mAllocInfo.pSetLayouts = &ref;
}

void DescriptorSet::create(VkDevice device)
{
	if (vkAllocateDescriptorSets(device, &mAllocInfo, &mHandle) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	std::vector<VkWriteDescriptorSet> vkWrites;
	vkWrites.reserve(mWrites.size());

	for (auto& write : mWrites)
	{
		write.mWrite.dstSet = mHandle;
		vkWrites.push_back(write.mWrite);
	}

	vkUpdateDescriptorSets(device, static_cast<uint32_t>(vkWrites.size()), vkWrites.data(), 0, nullptr);
}

void DescriptorSet::destroy(VkDevice device)
{
	// not used, destroyed by its pool
}
