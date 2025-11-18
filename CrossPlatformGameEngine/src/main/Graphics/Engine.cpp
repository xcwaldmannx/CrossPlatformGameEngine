#include "Engine.h"

using namespace ascen;

Engine::Engine(WindowManager& windowManager) :
	mWindowManager(windowManager),
	mCommandPoolFactory(VK_NULL_HANDLE),
	mDescriptorFactory(VK_NULL_HANDLE),
	mSwapchainFactory(VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE),
	mRenderPassFactory(VK_NULL_HANDLE, VK_NULL_HANDLE),
	mGraphicsPipelineFactory(VK_NULL_HANDLE),
	mComputePipelineFactory(VK_NULL_HANDLE),
	mBufferFactory(VK_NULL_HANDLE, VK_NULL_HANDLE),
	mTextureFactory(VK_NULL_HANDLE, VK_NULL_HANDLE),
	mSamplerFactory(VK_NULL_HANDLE, VK_NULL_HANDLE)
{
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
	mSurface = ascen::Surface::create(mInstance, mWindowManager.getWindow());

	mDebugMessenger = ascen::DebugMessenger::create(mInstance);

	mPhysicalDevice = ascen::PhysicalDevice::get(mInstance, mSurface);

	ascen::QueueFamilies::updateQueueFamilies(mPhysicalDevice, mSurface, mGraphicsFamily, mPresentFamily);

	mDevice = ascen::Device::create(mPhysicalDevice, mGraphicsFamily.value(), mPresentFamily.value());

	createSyncObjects();

	// factories
	mCommandPoolFactory      = CommandPoolFactory(mDevice);
	mDescriptorFactory       = DescriptorFactory(mDevice);
	mSwapchainFactory        = SwapchainFactory(mPhysicalDevice, mDevice, mSurface);
	mRenderPassFactory       = RenderPassFactory(mPhysicalDevice, mDevice);
	mGraphicsPipelineFactory = GraphicsPipelineFactory(mDevice);
	mComputePipelineFactory  = ComputePipelineFactory(mDevice);
	mBufferFactory           = BufferFactory(mPhysicalDevice, mDevice);
	mTextureFactory          = TextureFactory(mPhysicalDevice, mDevice);
	mSamplerFactory          = SamplerFactory(mPhysicalDevice, mDevice);

	// initialize ECS
	mEcs.registerComponent<TransformComponent>();
	mEcs.registerComponent<ModelComponent>();

	std::unordered_map<uint32_t, ModelData> modelData;

	auto readSig = mEcs.getSignature<TransformComponent, ModelComponent>();
	auto writeSig = mEcs.getSignature<ModelComponent>();
	mEcs.registerSystem<RenderSystem>(readSig, writeSig, &modelData);

	std::cout << "ecs init!\n";
}

const CommandPoolFactory& Engine::commandPool()
{
	return mCommandPoolFactory;
}

const DescriptorFactory& Engine::descriptor()
{
	return mDescriptorFactory;
}

const SwapchainFactory& Engine::swapchain()
{
	return mSwapchainFactory;
}

const RenderPassFactory& Engine::renderPass()
{
	return mRenderPassFactory;
}

const GraphicsPipelineFactory& Engine::graphicsPipeline()
{
	return mGraphicsPipelineFactory;
}

const ComputePipelineFactory& Engine::computePipeline()
{
	return mComputePipelineFactory;
}

const BufferFactory& Engine::buffer()
{
	return mBufferFactory;
}

const TextureFactory& Engine::texture()
{
	return mTextureFactory;
}

const SamplerFactory& Engine::sampler()
{
	return mSamplerFactory;
}

EcsSystem& Engine::ecs()
{
	return mEcs;
}

void Engine::resize(
	const CommandPoolPtr& commandPool,
	const RenderPassPtr renderPass,
	SwapchainPtr& swapchain,
	TexturePtr& depthTexture)
{
		vkDeviceWaitIdle(mDevice);

		swapchain->destroy(mDevice);

		swapchain = mSwapchainFactory.create(
			mWindowManager.getWindow(),
			mGraphicsFamily.value(),
			mPresentFamily.value());

		if (depthTexture)
		{
			depthTexture->destroy(mDevice);
		}

		depthTexture = texture().createDepth(
			mGraphicsFamily.value(), commandPool, swapchain->getExtent().width, swapchain->getExtent().height);

		swapchain->createFrameBuffers(
			mDevice,
			renderPass->handle(),
			depthTexture->handle());
}

void Engine::drawFrame(
	const CommandPoolPtr& commandPool,
	const RenderPassPtr& renderPass,
	const SwapchainPtr& swapchain,
	RenderGraph& renderGraph,
	const BufferPtr& indirectBuffer,
	const std::vector<IndirectBuffer::DrawCommand> drawCommands,
	bool& isResized)
{
	vkWaitForFences(mDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);

	VkResult nextImageResult = vkAcquireNextImageKHR(
		mDevice,
		swapchain->handle(),
		UINT64_MAX,
		mImageAvailableSemaphores[mCurrentFrame],
		VK_NULL_HANDLE,
		&mCurrentImage);

	if (nextImageResult == VK_ERROR_OUT_OF_DATE_KHR)
	{
		isResized = true;
		return;
	}
	else if (nextImageResult != VK_SUCCESS && nextImageResult != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swapchain image!");
	}

	vkResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);

	renderGraph.compile();

	commandPool->record(
		mPhysicalDevice,
		mCurrentFrame,
		mCurrentImage,
		renderGraph,
		indirectBuffer->handle(),
		renderPass,
		swapchain,
		drawCommands);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandPool->getBufferIndex(mCurrentFrame);

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

	VkSwapchainKHR swapChains[] = { swapchain->handle() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &mCurrentImage;
	presentInfo.pResults = nullptr;

	VkQueue presentQueue = ascen::QueueFamilies::getDeviceQueue(mDevice, mPresentFamily.value());
	VkResult queuePresentResult = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR)
	{
		isResized = true;
	}
	else if (queuePresentResult != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

uint32_t Engine::getGraphicsFamily() const
{
	return mGraphicsFamily.value();
}

uint32_t Engine::getPresentFamily() const
{
	return mPresentFamily.value();
}

void Engine::wait()
{
	vkDeviceWaitIdle(mDevice);
}

void Engine::cleanup()
{
	vkDeviceWaitIdle(mDevice);

	destroySyncObjects();
	ascen::Device::destroy(mDevice);
	ascen::Surface::destroy(mInstance, mSurface);
	ascen::DebugMessenger::destroy(mInstance, mDebugMessenger);
	ascen::Instance::destroy(mInstance);
}

void Engine::createSyncObjects()
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

void Engine::destroySyncObjects()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(mDevice, mRenderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(mDevice, mImageAvailableSemaphores[i], nullptr);
		vkDestroyFence(mDevice, mInFlightFences[i], nullptr);
	}
}
