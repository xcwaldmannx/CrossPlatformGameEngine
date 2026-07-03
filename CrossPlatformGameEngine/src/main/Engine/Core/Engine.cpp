#include "Engine.h"

#include "../../WindowManager/WindowManager.h"
#include "../Device/Physical/PhysicalDevice.h"

#include "../Swapchain/Swapchain.h"

using namespace ascen;

Engine::Engine(::WindowManager& windowManager) :
	mWindowManager(windowManager),
	mVulkanContext(windowManager),
	mRenderContext(windowManager, mVulkanContext),
	mResourceRegistry(mVulkanContext, mRenderContext),
	mDescriptorRegistry(mVulkanContext, mResourceRegistry),
	mPipelineRegistry(mVulkanContext, mRenderContext, mVertexRegistry, mDescriptorRegistry),
	mRenderTargetRegistry(mVulkanContext, mRenderContext),
	mRenderer(windowManager, mEcs, mVulkanContext, mRenderContext,
		mVertexRegistry, mResourceRegistry, mDescriptorRegistry,
		mPipelineRegistry, mFramePassRegistry, mRenderTargetRegistry)
{}

VertexRegistry& Engine::vertex()
{
	return mVertexRegistry;
}

ResourceRegistry& Engine::resource()
{
	return mResourceRegistry;
}

DescriptorRegistry& Engine::descriptor()
{
	return mDescriptorRegistry;
}

PipelineRegistry& Engine::pipeline()
{
	return mPipelineRegistry;
}

FramePassRegistry& Engine::frame()
{
	return mFramePassRegistry;
}

RenderTargetRegistry& Engine::render()
{
	return mRenderTargetRegistry;
}

EcsSystem& Engine::ecs()
{
	return mEcs;
}

void Engine::reload()
{
	mVertexRegistry.reconstruct();
	mResourceRegistry.reconstruct();
	mDescriptorRegistry.reconstruct();
	mPipelineRegistry.reconstruct();
	mFramePassRegistry.reconstruct();
	mRenderTargetRegistry.reconstruct();
}

void Engine::drawFrame()
{
	mRenderer.drawFrame();
}

void Engine::cleanup()
{
	mVulkanContext.waitIdle();

	mRenderer.cleanup();
	mRenderTargetRegistry.cleanup();
	mFramePassRegistry.cleanup();
	mPipelineRegistry.cleanup();
	mDescriptorRegistry.cleanup();
	mResourceRegistry.cleanup();
	mVertexRegistry.cleanup();
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
