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

	resize(mCommandPool, mRenderPass, mSwapchain, mDepthTexture);
}

void RenderContext::resize(
	const CommandPoolPtr& commandPool,
	const RenderPassPtr renderPass,
	SwapchainPtr& swapchain,
	TexturePtr& depthTexture)
{
	vkDeviceWaitIdle(mDevice);

	swapchain->destroy(mDevice);

	swapchain = mSwapchainFactory.create(
		mWindowManager.getWindow(),
		mGraphicsFamilyIndex,
		mPresentFamilyIndex);

	if (depthTexture)
	{
		depthTexture->destroy(mDevice);
	}

	depthTexture = mTextureFactory.createDepth(
		commandPool,
		swapchain->getExtent().width,
		swapchain->getExtent().height);

	swapchain->createFrameBuffers(
		mDevice,
		renderPass->handle(),
		depthTexture->handle());
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
