#include "DescriptorSetLayout.h"

#include <stdexcept>

using namespace vulkan;

void DescriptorSetLayout::init(VkDevice logicalDevice) {
	mLogicalDevice = logicalDevice;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = mDescriptorSets.data();

	if (vkCreateDescriptorSetLayout(mLogicalDevice, &layoutInfo, nullptr, &mDescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

DescriptorSet DescriptorSetLayout::createDescriptorSet(uint32_t slot, DescriptorType type, DescriptorStageFlags flags) {
	VkDescriptorSetLayoutBinding binding;
	binding.binding = slot;
	binding.descriptorType = (VkDescriptorType)type;
	binding.descriptorCount = 1;
	binding.stageFlags = (VkShaderStageFlagBits)flags;
	binding.pImmutableSamplers = nullptr;
	mDescriptorSets.push_back(binding);

	return mDescriptorSets.size() - 1;
}

void DescriptorSetLayout::destroy() {
	vkDestroyDescriptorSetLayout(mLogicalDevice, mDescriptorSetLayout, nullptr);
}

const VkDescriptorSetLayout& DescriptorSetLayout::get() const {
	return mDescriptorSetLayout;
}
