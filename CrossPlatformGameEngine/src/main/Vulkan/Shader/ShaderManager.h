#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace vulkan {

	class ShaderManager {
	public:
		VkShaderModule createShaderModule(const VkDevice& logicalDevice, std::vector<char> shaderCode) const;
		void destroyShaderModule(const VkDevice& logicalDevice, VkShaderModule module) const;
	private:
	};

}
