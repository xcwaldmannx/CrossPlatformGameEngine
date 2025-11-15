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

		template<std::derived_from<Vertex_I> T>
		GraphicsPipelinePtr create(
			const std::string& vertexShaderFilepath,
			const std::string& pixelShaderFilepath,
			const DescriptorSetLayoutPtr& descriptorSetLayout,
			const SwapchainPtr& swapchain,
			const RenderPassPtr& renderPass) const
		{
			GraphicsPipelinePtr ptr(new GraphicsPipeline<T>(
				vertexShaderFilepath,
				pixelShaderFilepath,
				descriptorSetLayout,
				swapchain,
				renderPass));
			ptr->create(mDevice);
			return ptr;
		}

	private:
		VkDevice mDevice = VK_NULL_HANDLE;
	};

}
