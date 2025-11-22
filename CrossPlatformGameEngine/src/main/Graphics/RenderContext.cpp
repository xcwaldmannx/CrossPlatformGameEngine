#include "RenderContext.h"

using namespace ascen;

RenderContext::RenderContext(
	WindowManager& windowManager,
	VulkanContext& context) :
	mWindowManager(windowManager),
	mDevice(context.getDevice()),
	mGraphicsFamilyIndex(context.getGraphicsFamilyIndex()),
	mPresentFamilyIndex(context.getPresentFamilyIndex()),
	mCommandPoolFactory(context.getCommandPoolFactory()),
	mSwapchainFactory(context.getSwapchainFactory()),
	mRenderPassFactory(context.getRenderPassFactory()),
	mTextureFactory(context.getTextureFactory())
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

const CommandPoolPtr& RenderContext::getCommandPool()
{
	return mCommandPool;
}

const SwapchainPtr& RenderContext::getSwapchain()
{
	return mSwapchain;
}

const RenderPassPtr& RenderContext::getRenderPass()
{
	return mRenderPass;
}
