#pragma once

#include "../../Graphics/Types.h"

#include "../../Graphics/Pipeline/ComputePipeline/ComputePipeline.h"

class MyComputePipeline : public ascen::ComputePipeline
{
public:
	MyComputePipeline(
		const std::string& computeShaderFilepath,
		const ascen::DescriptorSetLayoutPtr& descriptorSetLayout);

	void createDescriptorResources();
	void destroyDescriptorResources();
};