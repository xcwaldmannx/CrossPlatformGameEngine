#include "DescriptorPool.h"

#include <stdexcept>

using namespace ascen;

DescriptorPool::DescriptorPool(const std::vector<Size>& sizes)
{
	mCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	mCreateInfo.poolSizeCount = static_cast<uint32_t>(sizes.size());
	mCreateInfo.pPoolSizes = sizes.data();
	mCreateInfo.maxSets = 1;
}

void DescriptorPool::create(VkDevice device)
{
	if (vkCreateDescriptorPool(device, &mCreateInfo, nullptr, &mHandle) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void DescriptorPool::destroy(VkDevice device)
{
	vkDestroyDescriptorPool(device, mHandle, nullptr);
}
