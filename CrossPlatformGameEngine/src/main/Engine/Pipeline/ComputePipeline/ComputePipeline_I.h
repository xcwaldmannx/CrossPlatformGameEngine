#pragma once

#include "../Pipeline_I.h"

#include <string>

namespace ascen
{

	class ComputePipeline_I : public Pipeline_I
	{
	public:
		ComputePipeline_I(
			const pipeline::ComputeParams& params,
			const std::string& computeShaderFilepath) :
			mParams(params),
			mComputeShaderFilepath(computeShaderFilepath) {}

		virtual void destroy(VkDevice device) override = 0;

		virtual void uploadPushConstants(const VkCommandBuffer commandBuffer) = 0;

	protected:
		const pipeline::ComputeParams& mParams;
		const std::string mComputeShaderFilepath;
	};

}
