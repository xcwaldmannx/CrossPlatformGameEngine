#include "DescriptorSetLayout.h"

#include <stdexcept>

using namespace ascen;

DescriptorSetLayout::DescriptorSetLayout(
	VkDevice device,
	const std::vector<Binding>& bindings,
	const std::vector<uint32_t>& bindFlags)
{
	VkDescriptorSetLayoutBindingFlagsCreateInfo flagsInfo{};
	flagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;

	flagsInfo.bindingCount = static_cast<uint32_t>(bindFlags.size());
	flagsInfo.pBindingFlags = bindFlags.data();

	VkDescriptorSetLayoutCreateFlags layoutFlags = 0;

	for (const auto flags : bindFlags)
	{
		if (flags & VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT)
		{
			layoutFlags |=
				VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
			break;
		}
	}

	VkDescriptorSetLayoutCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	createInfo.pBindings = bindings.data();
	createInfo.pNext = &flagsInfo;
	createInfo.flags = layoutFlags;

	if (vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &mHandle) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void DescriptorSetLayout::destroy(VkDevice device)
{
	vkDestroyDescriptorSetLayout(device, mHandle, nullptr);
}
