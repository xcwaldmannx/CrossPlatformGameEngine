#include "HandleManager.h"

using namespace ascen;

void HandleManager::create(VkDevice device, Handle_I* handle)
{
	handle->create(device);
}

void HandleManager::destroy(VkDevice device)
{
	for (auto it = mHandles.rbegin(); it != mHandles.rend(); it++)
	{
		(*it)->destroy(device);
	}
}
