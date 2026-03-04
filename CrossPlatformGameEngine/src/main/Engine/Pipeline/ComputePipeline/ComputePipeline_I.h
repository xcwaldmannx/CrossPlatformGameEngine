#pragma once

#include "../Pipeline_I.h"

#include <string>

namespace ascen
{

	class ComputePipeline_I : public Pipeline_I
	{
	public:
		ComputePipeline_I(
			const std::string& computeShaderFilepath) :
			mComputeShaderFilepath(computeShaderFilepath) {}

		virtual void create(VkDevice device) override = 0;
		virtual void destroy(VkDevice device) override = 0;

	protected:
		const std::string mComputeShaderFilepath;
	};

}
