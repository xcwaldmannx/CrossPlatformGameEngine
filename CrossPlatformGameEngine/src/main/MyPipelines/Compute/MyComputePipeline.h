#pragma once

#include "../../Graphics/Pipeline/ComputePipeline/ComputePipeline.h"

class MyComputePipeline : public ascen::ComputePipeline
{
public:
	MyComputePipeline(
		const std::string& computeShaderFilepath,
		VkDescriptorSetLayout descriptorSetLayout);

	void createDescriptorResources();
	void destroyDescriptorResources();
};