#include "ShaderHandler.h"

#include <stdexcept>

#include <vulkan/vulkan.h>

/*
Begin ShaderHandler Implementation
*/

namespace ascen {

	void createShader(
		LogicalDevice& logicalDevice,
		const std::vector<char>& shaderCode,
		ShaderModule* module) {

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shaderCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

		if (vkCreateShaderModule(logicalDevice.mDevice, &createInfo, nullptr, &module->mShaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}
	}

	void destroyShader(LogicalDevice& device, ShaderModule& module) {
		vkDestroyShaderModule(device.mDevice, module.mShaderModule, nullptr);
	}

	void createShaderStage(
		VkDevice device,
		const std::vector<char>& byteCode,
		VkShaderStageFlagBits stageFlags,
		ShaderStage* stage)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = byteCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(byteCode.data());

		if (vkCreateShaderModule(device, &createInfo, nullptr, &stage->mModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader stage!");
		}

		stage->mInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stage->mInfo.stage = stageFlags;
		stage->mInfo.module = stage->mModule;
		stage->mInfo.pName = "main";
	}

	void destroyShaderStage(VkDevice device, ShaderStage& module) {
		vkDestroyShaderModule(device, module.mModule, nullptr);
	}

}

/*
End ShaderHandler Implementation
*/
