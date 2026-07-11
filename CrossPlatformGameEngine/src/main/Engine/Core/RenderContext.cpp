#include "RenderContext.h"

#include "../RenderPass/RenderPass.h"
#include "../Swapchain/Swapchain.h"
#include "../Resource/Texture/Texture.h"

using namespace ascen;

RenderContext::RenderContext(
	WindowManager& windowManager,
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

	resize();
}

void RenderContext::resize()
{
	if (WindowManager::getWidth() == 0 ||
		WindowManager::getHeight() == 0)
	{
		return;
	}

	vkDeviceWaitIdle(mDevice);

	if (mSwapchain)
	{
		mSwapchain->destroy(mDevice);
	}

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

	mSwapchain->setDepthTexture(mDepthTexture);

	mWindowManager.setResized(false);
}

void RenderContext::cleanup() const
{
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
