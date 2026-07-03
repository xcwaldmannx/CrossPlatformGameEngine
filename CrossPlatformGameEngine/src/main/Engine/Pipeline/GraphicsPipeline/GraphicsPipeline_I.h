#pragma once

#include "../Pipeline_I.h"

#include <string>

#include <vulkan/vulkan.h>

namespace ascen
{

	class GraphicsPipeline_I : public Pipeline_I
	{
	public:
		GraphicsPipeline_I(
			const pipeline::GraphicsParams& params,
			const std::string& vertexShaderFilepath,
			const std::string& pixelShaderFilepath) :
			mParams(params),
			mVertexShaderFilepath(vertexShaderFilepath),
			mPixelShaderFilepath(pixelShaderFilepath) {}

		virtual void destroy(VkDevice device) override = 0;

	protected:
		const pipeline::GraphicsParams& mParams;
		const std::string mVertexShaderFilepath;
		const std::string mPixelShaderFilepath;
	};

}
