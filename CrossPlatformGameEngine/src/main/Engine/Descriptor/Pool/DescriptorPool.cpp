#include "DescriptorPool.h"

#include <stdexcept>

using namespace ascen;

DescriptorPool::DescriptorPool(VkDevice device, const std::vector<Size>& sizes)
{
	VkDescriptorPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.poolSizeCount = static_cast<uint32_t>(sizes.size());
	createInfo.pPoolSizes = sizes.data();
	createInfo.maxSets = 16;

	if (vkCreateDescriptorPool(device, &createInfo, nullptr, &mHandle) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void DescriptorPool::destroy(VkDevice device)
{
	vkDestroyDescriptorPool(device, mHandle, nullptr);
}
