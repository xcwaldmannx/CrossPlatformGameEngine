#include "DescriptorSetLayout.h"

#include <stdexcept>

using namespace ascen;

DescriptorSetLayout::DescriptorSetLayout(
	VkDevice device,
	const std::vector<Binding>& bindings)
{
	VkDescriptorSetLayoutCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	createInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &mHandle) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void DescriptorSetLayout::destroy(VkDevice device)
{
	vkDestroyDescriptorSetLayout(device, mHandle, nullptr);
}
