#include "Engine.h"

#include "../../WindowManager/WindowManager.h"
#include "../Device/Physical/PhysicalDevice.h"

#include "../Swapchain/Swapchain.h"

using namespace ascen;

Engine::Engine(::WindowManager& windowManager) :
	mWindowManager(windowManager),
	mVulkanContext(windowManager),
	mRenderContext(windowManager, mVulkanContext),
	mRegistryManager(mVulkanContext, mRenderContext),
	mRenderer(windowManager, mEcs, mVulkanContext, mRenderContext, mRegistryManager) {}

void Engine::uploadBuffer(
	const std::string& name,
	const void* items,
	const uint32_t itemCount,
	const uint32_t itemSize,
	const uint32_t offset) const
{
	mRegistryManager.uploadBuffer(name, items, itemCount, itemSize, offset);
}

void Engine::uploadTexture(const std::string& name, const std::vector<unsigned char>& pixels) const
{
	mRegistryManager.uploadTexture(name, pixels);
}

void Engine::updateTransfer(
			const std::string& name,
			uint32_t transferId,
			const std::variant<transfer::BufferRegion, transfer::ImageRegion, transfer::BufferImageRegion>& region)
{
	mRegistryManager.updateTransfer(name, transferId, region);
}

EcsSystem& Engine::ecs()
{
	return mEcs;
}

void Engine::reload()
{
	mRegistryManager.reconstruct();
}

void Engine::drawFrame()
{
	mRenderer.drawFrame();
}

void Engine::cleanup()
{
	mVulkanContext.waitIdle();

	mRegistryManager.deconstruct();
	mRenderer.cleanup();
	mRenderContext.cleanup();
	mVulkanContext.cleanup();
}

uint32_t Engine::getScreenWidth() const
{
	return mRenderContext.getSwapchain()->getExtent().width;
}

uint32_t Engine::getScreenHeight() const
{
	return mRenderContext.getSwapchain()->getExtent().height;
}

Format Engine::getDepthFormat() const
{
	return static_cast<Format>(PhysicalDevice::findDepthFormat(mVulkanContext.getPhysicalDevice()));
}

uint32_t Engine::getFrameIndex() const
{
	return mRenderer.getFrameIndex();
}

const std::vector<VkImage>& Engine::getPresentImages() const
{
	return mRenderContext.getSwapchain()->getImages();
}

Format Engine::getImageFormat() const
{
	return static_cast<Format>(mRenderContext.getSwapchain()->getImageFormat());
}
