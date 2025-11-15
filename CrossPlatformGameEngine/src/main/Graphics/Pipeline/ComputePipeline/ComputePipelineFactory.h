#pragma once

#include "ComputePipeline.h"
#include "../../Vertex/Vertex_I.h"
#include "../../Types.h"

#include <concepts>
#include <memory>

#include <vulkan/vulkan.h>

namespace ascen
{

	class ComputePipelineFactory
	{
	public:
		ComputePipelineFactory(VkDevice device);

		ComputePipelinePtr create(
			const std::string& computeShaderFilepath,
			const DescriptorSetLayoutPtr& descriptorSetLayout) const;

	private:
		VkDevice mDevice = VK_NULL_HANDLE;
	};

}
