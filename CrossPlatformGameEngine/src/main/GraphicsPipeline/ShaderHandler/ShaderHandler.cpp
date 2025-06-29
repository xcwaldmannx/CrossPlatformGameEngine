#include "ShaderHandler.h"

#include <stdexcept>

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

	void destroyShader(LogicalDevice& logicalDevice, ShaderModule& module) {
		vkDestroyShaderModule(logicalDevice.mDevice, module.mShaderModule, nullptr);
	}

}

/*
End ShaderHandler Implementation
*/
