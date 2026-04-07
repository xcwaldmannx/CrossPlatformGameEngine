#include "ComputePipelineFactory.h"

#include "ComputePipeline.h"

using namespace ascen;

ComputePipelineFactory::ComputePipelineFactory(VkDevice device) : mDevice(device) {}

ComputePipelinePtr ComputePipelineFactory::create(
	const ComputePipelineParams& params,
	const std::string& computeShaderFilepath,
	const std::vector<DescriptorSetLayoutPtr>& descriptorSetLayouts) const
{
	ComputePipelinePtr ptr(new ComputePipeline(
		mDevice,
		params,
		computeShaderFilepath,
		descriptorSetLayouts));

	return ptr;
}
