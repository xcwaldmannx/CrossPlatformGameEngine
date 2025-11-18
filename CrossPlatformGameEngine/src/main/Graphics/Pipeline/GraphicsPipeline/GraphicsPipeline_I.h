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
			const std::string& vertexShaderFilepath,
			const std::string& pixelShaderFilepath) :
			mVertexShaderFilepath(vertexShaderFilepath),
			mPixelShaderFilepath(pixelShaderFilepath) {}

		virtual void create(VkDevice device) override = 0;
		virtual void destroy(VkDevice device) override = 0;

	protected:
		const std::string mVertexShaderFilepath;
		const std::string mPixelShaderFilepath;
	};

}
