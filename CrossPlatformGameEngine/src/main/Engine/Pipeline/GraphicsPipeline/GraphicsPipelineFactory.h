#pragma once

#include "../../Core/Types.h"

#include "../GraphicsPipeline/GraphicsPipeline_I.h"

#include <concepts>
#include <memory>

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

namespace ascen
{

	class GraphicsPipelineFactory
	{
	public:
		GraphicsPipelineFactory(VkDevice device);

		GraphicsPipelinePtr create(
			const pipeline::GraphicsParams& params,
			const std::string& vertexShaderFilepath,
			const std::string& pixelShaderFilepath,
			const VertexPtr& vertex,
			const std::vector<DescriptorSetLayoutPtr>& descriptorSetLayouts,
			const SwapchainPtr& swapchain,
			const RenderPassPtr& renderPass) const;

	private:
		VkDevice mDevice = VK_NULL_HANDLE;
	};

}
