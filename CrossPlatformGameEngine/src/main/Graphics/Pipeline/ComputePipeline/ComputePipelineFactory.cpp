#include "ComputePipelineFactory.h"

using namespace ascen;

ComputePipelineFactory::ComputePipelineFactory(VkDevice device) : mDevice(device) {}

ComputePipelinePtr ComputePipelineFactory::create(
	const std::string& computeShaderFilepath,
	const std::vector<DescriptorSetLayoutPtr>& descriptorSetLayouts) const
{
	ComputePipelinePtr ptr(new ComputePipeline(
		mDevice,
		computeShaderFilepath,
		descriptorSetLayouts));
	ptr->create(mDevice);
	return ptr;
}
