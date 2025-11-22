#include "VulkanContext.h"

using namespace ascen;

VulkanContext::VulkanContext(WindowManager& windowManager) :
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
	if (ValidationLayers::isEnabled())
	{
		ValidationLayers::add("VK_LAYER_KHRONOS_validation", &mValidationLayers);
		ValidationLayers::validate(mValidationLayers);

		Extensions::add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, &mExtensions);
	}

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	Extensions::add(glfwExtensions, glfwExtensionCount, &mExtensions);
	Extensions::validate(mExtensions);

	mInstance = Instance::create(mValidationLayers, mExtensions);
	mSurface = Surface::create(mInstance, mWindowManager.getWindow());
	mDebugMessenger = DebugMessenger::create(mInstance);
	mPhysicalDevice = PhysicalDevice::get(mInstance, mSurface);

	QueueFamily graphicsFamily{};
	QueueFamily presentFamily{};
	QueueFamilies::updateQueueFamilies(mPhysicalDevice, mSurface, graphicsFamily, presentFamily);
	mGraphicsFamilyIndex = graphicsFamily.value();
	mPresentFamilyIndex = presentFamily.value();
	mGraphicsQueue = QueueFamilies::getDeviceQueue(mDevice, mGraphicsFamilyIndex);
	mGraphicsQueue = QueueFamilies::getDeviceQueue(mDevice, mPresentFamilyIndex);

	mDevice = Device::create(mPhysicalDevice, graphicsFamily.value(), presentFamily.value());

	mCommandPoolFactory      = CommandPoolFactory(mDevice);
	mDescriptorFactory       = DescriptorFactory(mDevice);
	mSwapchainFactory        = SwapchainFactory(mPhysicalDevice, mDevice, mSurface);
	mRenderPassFactory       = RenderPassFactory(mPhysicalDevice, mDevice);
	mGraphicsPipelineFactory = GraphicsPipelineFactory(mDevice);
	mComputePipelineFactory  = ComputePipelineFactory(mDevice);
	mBufferFactory           = BufferFactory(mPhysicalDevice, mDevice);
	mTextureFactory          = TextureFactory(mPhysicalDevice, mDevice);
	mSamplerFactory          = SamplerFactory(mPhysicalDevice, mDevice);
}

VkPhysicalDevice VulkanContext::getPhysicalDevice() const
{
	return mPhysicalDevice;
}

VkDevice VulkanContext::getDevice() const
{
	return mDevice;
}

uint32_t VulkanContext::getGraphicsFamilyIndex() const
{
	return mGraphicsFamilyIndex;
}

uint32_t VulkanContext::getPresentFamilyIndex() const
{
	return mPresentFamilyIndex;
}

VkQueue VulkanContext::getGraphicsQueue() const
{
	return mGraphicsQueue;
}

VkQueue VulkanContext::getPresentQueue() const
{
	return mPresentQueue;
}

const CommandPoolFactory& VulkanContext::getCommandPoolFactory() const
{
	return mCommandPoolFactory;
}

const DescriptorFactory& VulkanContext::getDescriptorFactory() const
{
	return mDescriptorFactory;
}

const SwapchainFactory& VulkanContext::getSwapchainFactory() const
{
	return mSwapchainFactory;
}

const RenderPassFactory& VulkanContext::getRenderPassFactory() const
{
	return mRenderPassFactory;
}

const GraphicsPipelineFactory& VulkanContext::getGraphicsPipelineFactory() const
{
	return mGraphicsPipelineFactory;
}

const ComputePipelineFactory& VulkanContext::getComputePipelineFactory() const
{
	return mComputePipelineFactory;
}

const BufferFactory& VulkanContext::getBufferFactory() const
{
	return mBufferFactory;
}

const TextureFactory& VulkanContext::getTextureFactory() const
{
	return mTextureFactory;
}

const SamplerFactory& VulkanContext::getSamplerFactory() const
{
	return mSamplerFactory;
}

void VulkanContext::waitIdle() const
{
	vkDeviceWaitIdle(mDevice);
}

void VulkanContext::cleanup() const
{
	ascen::Device::destroy(mDevice);
	ascen::Surface::destroy(mInstance, mSurface);
	ascen::DebugMessenger::destroy(mInstance, mDebugMessenger);
	ascen::Instance::destroy(mInstance);
}
