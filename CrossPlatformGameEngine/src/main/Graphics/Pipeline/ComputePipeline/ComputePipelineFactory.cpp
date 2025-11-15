#include "ComputePipelineFactory.h"

using namespace ascen;

ComputePipelineFactory::ComputePipelineFactory(VkDevice device) : mDevice(device) {}

ComputePipelinePtr ComputePipelineFactory::create(
	const std::string& computeShaderFilepath,
	const DescriptorSetLayoutPtr& descriptorSetLayout) const
{
	ComputePipelinePtr ptr(new ComputePipeline(
		computeShaderFilepath,
		descriptorSetLayout));
	ptr->create(mDevice);
	return ptr;
}
