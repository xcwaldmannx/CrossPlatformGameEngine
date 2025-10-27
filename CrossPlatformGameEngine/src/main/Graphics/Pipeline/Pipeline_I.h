#pragma once

#include "../HandleManager/Handle.h"

#include <string>

namespace ascen
{

	class Pipeline_I : public Handle<VkPipeline>
	{
	public:
		Pipeline_I(
			const std::string vertexShaderFilepath,
			const std::string pixelShaderFilepath)
			: mVertexShaderFilepath(vertexShaderFilepath),
			mPixelShaderFilepath(pixelShaderFilepath) {}

		VkPipelineLayout getLayout() const
		{
			return mLayout;
		}

	protected:
		VkPipelineLayout mLayout{};
		const std::string mVertexShaderFilepath;
		const std::string mPixelShaderFilepath;
	};

}