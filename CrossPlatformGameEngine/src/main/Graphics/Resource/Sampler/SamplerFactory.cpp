#include "SamplerFactory.h"

using namespace ascen;

SamplerFactory::SamplerFactory(VkPhysicalDevice physicalDevice, VkDevice device) :
	mPhysicalDevice(physicalDevice), mDevice(device) {}

SamplerPtr SamplerFactory::createSampler() const
{
	return std::make_shared<Sampler2>(mPhysicalDevice, mDevice);
}
