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

	protected:
		const std::string mVertexShaderFilepath;
		const std::string mPixelShaderFilepath;
	};

}
