#include "Engine.h"

using namespace ascen;


Engine::Engine(WindowManager* windowManager) :
	mWindowManager(windowManager),
	mCommandPoolFactory(VK_NULL_HANDLE),
	mDescriptorFactory(VK_NULL_HANDLE),
	mSwapchainFactory(VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE),
	mRenderPassFactory(VK_NULL_HANDLE, VK_NULL_HANDLE),
	mGraphicsPipelineFactory(VK_NULL_HANDLE),
	mComputePipelineFactory(VK_NULL_HANDLE),
	mBufferFactory(VK_NULL_HANDLE, VK_NULL_HANDLE)
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
	mSurface = ascen::Surface::create(mInstance, mWindowManager->getWindow());

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

void Engine::destroyBuffer(const std::shared_ptr<Buffer>& buffer) const
{
	Buffer::destroy(mDevice, *buffer);
}

void Engine::resize(
	const CommandPoolPtr& commandPool,
	const RenderPassPtr renderPass,
	SwapchainPtr& swapchain,
	std::shared_ptr<Texture>& depthTexture)
{
		vkDeviceWaitIdle(mDevice);

		swapchain->destroy(mDevice);

		swapchain = mSwapchainFactory.create(
			mWindowManager->getWindow(),
			mGraphicsFamily.value(),
			mPresentFamily.value());

		if (depthTexture)
		{
			ascen::Texture::destroy(mDevice, *depthTexture);
		}

		depthTexture = std::make_shared<ascen::Texture>(
			ascen::Texture::create(
				mPhysicalDevice,
				mDevice,
				ascen::QueueFamilies::getDeviceQueue(mDevice, mGraphicsFamily.value()),
				commandPool,
				ascen::PhysicalDevice::findDepthFormat(mPhysicalDevice),
				VK_IMAGE_TILING_OPTIMAL,
				{},
				swapchain->getExtent().width,
				swapchain->getExtent().height,
				1,
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				VK_IMAGE_ASPECT_DEPTH_BIT
			)
		);

		swapchain->createFrameBuffers(
			mDevice,
			renderPass->handle(),
			depthTexture->getView());

		mIsWindowResized = false;
}

uint32_t Engine::getGraphicsFamily() const
{
	return mGraphicsFamily.value();
}

uint32_t Engine::getPresentFamily() const
{
	return mPresentFamily.value();
}

std::shared_ptr<Texture> Engine::createTexture(
	const std::shared_ptr<CommandPool>& commandPool,
	const std::vector<unsigned char>& pixels,
	uint32_t width,
	uint32_t height,
	uint32_t layers)
{
	return std::make_shared<Texture>(
		Texture::create(
			mPhysicalDevice,
			mDevice,
			ascen::QueueFamilies::getDeviceQueue(mDevice, mGraphicsFamily.value()),
			commandPool,
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_TILING_OPTIMAL,
			pixels,
			width,
			height,
			layers,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT));
}

void Engine::destroyTexture(const std::shared_ptr<Texture>& texture) const
{
	Texture::destroy(mDevice, *texture);
}

std::shared_ptr<Sampler> Engine::createSampler()
{
	return std::make_shared<Sampler>(Sampler::create(mPhysicalDevice, mDevice));
}

void Engine::destroySampler(const std::shared_ptr<Sampler>& sampler) const
{
	Sampler::destroy(mDevice, *sampler);
}
