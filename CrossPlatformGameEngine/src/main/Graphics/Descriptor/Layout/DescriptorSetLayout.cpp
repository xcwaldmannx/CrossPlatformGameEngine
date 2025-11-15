#include "DescriptorSetLayout.h"

#include <stdexcept>

using namespace ascen;

DescriptorSetLayout::DescriptorSetLayout(
	const std::vector<Binding>& bindings)
{
	mCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	mCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	mCreateInfo.pBindings = bindings.data();
}

void DescriptorSetLayout::create(VkDevice device)
{
	if (vkCreateDescriptorSetLayout(
		device, &mCreateInfo, nullptr, &mHandle) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void DescriptorSetLayout::destroy(VkDevice device)
{
	vkDestroyDescriptorSetLayout(device, mHandle, nullptr);
}
