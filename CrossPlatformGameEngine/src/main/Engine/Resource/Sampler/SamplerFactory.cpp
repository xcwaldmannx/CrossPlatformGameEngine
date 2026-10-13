#include "SamplerFactory.h"

#include "Sampler.h"

using namespace ascen;

SamplerFactory::SamplerFactory(VkPhysicalDevice physicalDevice, VkDevice device) :
	mPhysicalDevice(physicalDevice), mDevice(device) {}

SamplerPtr SamplerFactory::create() const
{
	return std::make_shared<Sampler>(mPhysicalDevice, mDevice);
}
