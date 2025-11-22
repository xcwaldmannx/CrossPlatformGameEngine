#pragma once

#include "GraphicsPipeline.h"
#include "../../Vertex/Vertex_I.h"
#include "../../Types.h"

#include <concepts>
#include <memory>

#include <vulkan/vulkan.h>

namespace ascen
{

	class GraphicsPipelineFactory
	{
	public:
		GraphicsPipelineFactory(VkDevice device);

		GraphicsPipelinePtr create(
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
