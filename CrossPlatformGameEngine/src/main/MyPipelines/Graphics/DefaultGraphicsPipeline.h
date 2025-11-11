#pragma once

#include "DefaultGraphicsPipeline_I.h"

#include <stdexcept>

template<std::derived_from<ascen::Vertex_I> T>
class DefaultGraphicsPipeline : public DefaultGraphicsPipeline_I
{
public:
	DefaultGraphicsPipeline(
		WindowManager* windowManager,
		const std::string& vertexShaderFilepath,
		const std::string& pixelShaderFilepath) :
		DefaultGraphicsPipeline_I(windowManager, vertexShaderFilepath, pixelShaderFilepath) {}

	void create() override
	{
		glfwSwapInterval(1);

		if (ascen::ValidationLayers::isEnabled())
		{
			ascen::ValidationLayers::add("VK_LAYER_KHRONOS_validation", &mValidationLayers);
			ascen::ValidationLayers::validate(mValidationLayers);

			ascen::Extensions::add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, &mExtensions);
		}

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		ascen::Extensions::add(glfwExtensions, glfwExtensionCount, &mExtensions);
		ascen::Extensions::validate(mExtensions);

		mInstance = ascen::Instance::create(mValidationLayers, mExtensions);
		mSurface = ascen::Surface::create(mInstance, mWindowManager->getWindow());

		mDebugMessenger = ascen::DebugMessenger::create(mInstance);

		mPhysicalDevice = ascen::PhysicalDevice::get(mInstance, mSurface);

		ascen::QueueFamilies::updateQueueFamilies(mPhysicalDevice, mSurface, mGraphicsFamily, mPresentFamily);

		mDevice = ascen::Device::create(mPhysicalDevice, mGraphicsFamily.value(), mPresentFamily.value());

		mSwapchain = std::make_shared<ascen::Swapchain>(
			mWindowManager->getWindow(),
			mPhysicalDevice,
			mSurface,
			mGraphicsFamily.value(),
			mPresentFamily.value());
		mSwapchain->create(mDevice);

		mRenderPass = std::make_shared<ascen::RenderPass>(
			mPhysicalDevice,
			VK_FORMAT_R8G8B8A8_SRGB,
			ascen::PhysicalDevice::findDepthFormat(mPhysicalDevice));
		mRenderPass->create(mDevice);

		mCommandPool = std::make_shared<ascen::CommandPool>(mGraphicsFamily.value());
		mCommandPool->create(mDevice);

		createSyncObjects();

		resize();

		createShaderResources();

		createDescriptorResources();

		mPipeline = std::make_shared<ascen::GraphicsPipeline<T>>(
			mVertexShaderFilepath,
			mPixelShaderFilepath,
			mSwapchain->getExtent(),
			mDescriptorSetLayout,
			mRenderPass->handle());
		mPipeline->create(mDevice);
	}

	void destroy() override
	{
		vkDeviceWaitIdle(mDevice);

		destroyShaderResources();

		ascen::Texture::destroy(mDevice, *mDepthTexture);

		destroySyncObjects();

		destroyDescriptorResources();

		mPipeline->destroy(mDevice);
		mCommandPool->destroy(mDevice);
		mRenderPass->destroy(mDevice);
		mSwapchain->destroy(mDevice);

		ascen::Device::destroy(mDevice);
		ascen::Surface::destroy(mInstance, mSurface);
		ascen::DebugMessenger::destroy(mInstance, mDebugMessenger);
		ascen::Instance::destroy(mInstance);
	}

	void submit(std::vector<VkDrawIndexedIndirectCommand> drawCommands) override
	{
		mDrawCommands = drawCommands;
	}

	void drawFrame() override
	{
		vkWaitForFences(mDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);

		uint32_t currentImage;
		VkResult nextImageResult = vkAcquireNextImageKHR(
			mDevice,
			mSwapchain->handle(),
			UINT64_MAX,
			mImageAvailableSemaphores[mCurrentFrame],
			VK_NULL_HANDLE,
			&currentImage);

		if (nextImageResult == VK_ERROR_OUT_OF_DATE_KHR)
		{
			mIsWindowResized = true;
			return;
		}
		else if (nextImageResult != VK_SUCCESS && nextImageResult != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swapchain image!");
		}

		vkResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);

		record(currentImage);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = mCommandPool->getBufferIndex(mCurrentFrame);

		VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VkQueue graphicsQueue = ascen::QueueFamilies::getDeviceQueue(mDevice, mGraphicsFamily.value());

		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, mInFlightFences[mCurrentFrame]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { mSwapchain->handle() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &currentImage;
		presentInfo.pResults = nullptr;

		VkQueue presentQueue = ascen::QueueFamilies::getDeviceQueue(mDevice, mPresentFamily.value());
		VkResult queuePresentResult = vkQueuePresentKHR(presentQueue, &presentInfo);

		if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR)
		{
			mIsWindowResized = true;
		}
		else if (queuePresentResult != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}

		mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

		mDrawCommands.clear();
	}

	void resize() override
	{
		vkDeviceWaitIdle(mDevice);

		mSwapchain->destroy(mDevice);

		mSwapchain.reset();
		mSwapchain = std::make_shared<ascen::Swapchain>(
			mWindowManager->getWindow(),
			mPhysicalDevice,
			mSurface,
			mGraphicsFamily.value(),
			mPresentFamily.value());

		mSwapchain->create(mDevice);

		if (mDepthTexture)
		{
			ascen::Texture::destroy(mDevice, *mDepthTexture);
		}

		mDepthTexture = std::make_shared<ascen::Texture>(
			ascen::Texture::create(
				mPhysicalDevice,
				mDevice,
				ascen::QueueFamilies::getDeviceQueue(mDevice, mGraphicsFamily.value()),
				mCommandPool,
				ascen::PhysicalDevice::findDepthFormat(mPhysicalDevice),
				VK_IMAGE_TILING_OPTIMAL,
				{},
				mSwapchain->getExtent().width,
				mSwapchain->getExtent().height,
				1,
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				VK_IMAGE_ASPECT_DEPTH_BIT
			)
		);

		mSwapchain->createFrameBuffers(
			mDevice,
			mRenderPass->handle(),
			mDepthTexture->getView());

		mIsWindowResized = false;
	}

	bool isResized() const override
	{
		return mIsWindowResized;
	}

protected:
	void createShaderResources() override
	{

	}

	void destroyShaderResources() override
	{

	}

	void createDescriptorResources() override
	{

	}

	void destroyDescriptorResources() override
	{

	}

	void createSyncObjects() override
	{
		mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		mRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(mDevice, &fenceInfo, nullptr, &mInFlightFences[i]) != VK_SUCCESS)
			{

				throw std::runtime_error("failed to create synchronization objects for a frame!");
			}
		}
	}

	void destroySyncObjects() override
	{
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(mDevice, mRenderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(mDevice, mImageAvailableSemaphores[i], nullptr);
			vkDestroyFence(mDevice, mInFlightFences[i], nullptr);
		}
	}
};
