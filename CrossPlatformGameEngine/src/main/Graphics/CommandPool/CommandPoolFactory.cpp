#include "CommandPoolFactory.h"

#include "CommandPool.h"

using namespace ascen;

CommandPoolFactory::CommandPoolFactory(VkDevice device) : mDevice(device) {}

CommandPoolPtr CommandPoolFactory::create(uint32_t queueFamilyIndex) const
{
	CommandPoolPtr ptr(new CommandPool(mDevice, queueFamilyIndex));
	return ptr;
}
