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
	mTextureFactory(VK_NULL_HANDLE, VK_NULL_HANDLE)
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

	// factories
	mCommandPoolFactory      = CommandPoolFactory(mDevice);
	mDescriptorFactory       = DescriptorFactory(mDevice);
	mSwapchainFactory        = SwapchainFactory(mPhysicalDevice, mDevice, mSurface);
	mRenderPassFactory       = RenderPassFactory(mPhysicalDevice, mDevice);
	mGraphicsPipelineFactory = GraphicsPipelineFactory(mDevice);
	mComputePipelineFactory  = ComputePipelineFactory(mDevice);
	mBufferFactory           = BufferFactory(mPhysicalDevice, mDevice);
	mTextureFactory          = TextureFactory(mPhysicalDevice, mDevice);
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

uint32_t Engine::getGraphicsFamily() const
{
	return mGraphicsFamily.value();
}

uint32_t Engine::getPresentFamily() const
{
	return mPresentFamily.value();
}

std::shared_ptr<Sampler> Engine::createSampler()
{
	return std::make_shared<Sampler>(Sampler::create(mPhysicalDevice, mDevice));
}

void Engine::destroySampler(const std::shared_ptr<Sampler>& sampler) const
{
	Sampler::destroy(mDevice, *sampler);
}

void Engine::cleanup() const
{
	ascen::Device::destroy(mDevice);
	ascen::Surface::destroy(mInstance, mSurface);
	ascen::DebugMessenger::destroy(mInstance, mDebugMessenger);
	ascen::Instance::destroy(mInstance);
}
