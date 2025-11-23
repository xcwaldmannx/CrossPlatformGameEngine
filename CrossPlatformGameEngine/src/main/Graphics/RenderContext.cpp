#include "RenderContext.h"

using namespace ascen;

RenderContext::RenderContext(
	const WindowManager& windowManager,
	const VulkanContext& vulkanContext) :
	mWindowManager(windowManager),
	mDevice(vulkanContext.getDevice()),
	mGraphicsFamilyIndex(vulkanContext.getGraphicsFamilyIndex()),
	mPresentFamilyIndex(vulkanContext.getPresentFamilyIndex()),
	mCommandPoolFactory(vulkanContext.getCommandPoolFactory()),
	mSwapchainFactory(vulkanContext.getSwapchainFactory()),
	mRenderPassFactory(vulkanContext.getRenderPassFactory()),
	mTextureFactory(vulkanContext.getTextureFactory())
{
	mCommandPool = mCommandPoolFactory.create(mGraphicsFamilyIndex);

	mSwapchain = mSwapchainFactory.create(
		mWindowManager.getWindow(),
		mGraphicsFamilyIndex,
		mPresentFamilyIndex);

	mRenderPass = mRenderPassFactory.create(VK_FORMAT_R8G8B8A8_SRGB);

	resize();
}

void RenderContext::resize()
{
	vkDeviceWaitIdle(mDevice);

	mSwapchain->destroy(mDevice);

	mSwapchain = mSwapchainFactory.create(
		mWindowManager.getWindow(),
		mGraphicsFamilyIndex,
		mPresentFamilyIndex);

	if (mDepthTexture)
	{
		mDepthTexture->destroy(mDevice);
	}

	mDepthTexture = mTextureFactory.createDepth(
		mCommandPool,
		mSwapchain->getExtent().width,
		mSwapchain->getExtent().height);

	mSwapchain->createFrameBuffers(
		mDevice,
		mRenderPass->handle(),
		mDepthTexture->handle());
}

void RenderContext::cleanup()
{
	mRenderPass->destroy(mDevice);
	mDepthTexture->destroy(mDevice);
	mSwapchain->destroy(mDevice);
	mCommandPool->destroy(mDevice);
}

const CommandPoolPtr& RenderContext::getCommandPool() const
{
	return mCommandPool;
}

const SwapchainPtr& RenderContext::getSwapchain() const
{
	return mSwapchain;
}

const RenderPassPtr& RenderContext::getRenderPass() const
{
	return mRenderPass;
}
