#pragma once

#include "../Pipeline_I.h"

#include <string>

#include <vulkan/vulkan.h>

namespace ascen
{

	struct GraphicsPipelineParams : PipelineParams
	{
		uint32_t mTopologyMode = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		uint32_t mPolygonMode = VK_POLYGON_MODE_FILL;
		uint32_t mCullMode = VK_CULL_MODE_BACK_BIT;
	};

	class GraphicsPipeline_I : public Pipeline_I
	{
	public:
		GraphicsPipeline_I(
			const GraphicsPipelineParams& params,
			const std::string& vertexShaderFilepath,
			const std::string& pixelShaderFilepath) :
		mParams(params),
			mVertexShaderFilepath(vertexShaderFilepath),
			mPixelShaderFilepath(pixelShaderFilepath) {}

		virtual void destroy(VkDevice device) override = 0;

	protected:
		const GraphicsPipelineParams& mParams;
		const std::string mVertexShaderFilepath;
		const std::string mPixelShaderFilepath;
	};

}
