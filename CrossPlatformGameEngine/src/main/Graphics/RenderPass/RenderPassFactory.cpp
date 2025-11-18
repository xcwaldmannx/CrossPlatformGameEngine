#include "RenderPassFactory.h"

#include "../Device/Physical/PhysicalDevice.h"

using namespace ascen;

RenderPassFactory::RenderPassFactory(
	VkPhysicalDevice physicalDevice,
	VkDevice device) :
	mPhysicalDevice(physicalDevice),
	mDevice(device) {}

RenderPassPtr RenderPassFactory::create(
	VkFormat colorFormat) const
{
	VkFormat depthFormat = PhysicalDevice::findDepthFormat(mPhysicalDevice);
	RenderPassPtr ptr(new RenderPass(mPhysicalDevice, colorFormat, depthFormat));
	ptr->create(mDevice);
	return ptr;
}