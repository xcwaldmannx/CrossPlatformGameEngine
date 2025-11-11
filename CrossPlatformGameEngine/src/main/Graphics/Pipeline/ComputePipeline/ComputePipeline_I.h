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

	protected:
		const std::string mComputeShaderFilepath;
	};

}
