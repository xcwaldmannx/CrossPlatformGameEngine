#include "SwapchainFactory.h"

using namespace ascen;

SwapchainFactory::SwapchainFactory(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	VkSurfaceKHR surface) :
	mPhysicalDevice(physicalDevice),
	mDevice(device),
	mSurface(surface) {}

SwapchainPtr SwapchainFactory::create(
	GLFWwindow* window,
	uint32_t graphicsFamily,
	uint32_t presentFamily) const
{
	SwapchainPtr ptr(new Swapchain(window, mPhysicalDevice, mSurface, graphicsFamily, presentFamily));
	ptr->create(mDevice);
	return ptr;
}