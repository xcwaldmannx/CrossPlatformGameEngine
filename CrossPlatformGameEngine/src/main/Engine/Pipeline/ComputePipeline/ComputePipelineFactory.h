#pragma once

#include "../../Core/Types.h"

#include <memory>

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	class ComputePipelineFactory
	{
	public:
		ComputePipelineFactory(VkDevice device);

		ComputePipelinePtr create(
			const pipeline::ComputeParams& params,
			const std::string& computeShaderFilepath,
			const std::vector<DescriptorSetLayoutPtr>& descriptorSetLayout) const;

	private:
		VkDevice mDevice = VK_NULL_HANDLE;
	};

}
