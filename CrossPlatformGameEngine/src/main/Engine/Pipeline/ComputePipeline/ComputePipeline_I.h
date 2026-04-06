#pragma once

#include "../Pipeline_I.h"

#include <string>

namespace ascen
{

	struct ComputePipelineParams : PipelineParams
	{

	};

	class ComputePipeline_I : public Pipeline_I
	{
	public:
		ComputePipeline_I(
			const ComputePipelineParams& params,
			const std::string& computeShaderFilepath) :
			mParams(params),
			mComputeShaderFilepath(computeShaderFilepath) {}

		virtual void create(VkDevice device) override = 0;
		virtual void destroy(VkDevice device) override = 0;

	protected:
		const ComputePipelineParams& mParams;
		const std::string mComputeShaderFilepath;
	};

}
