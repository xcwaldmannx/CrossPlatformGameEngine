#include "VulkanContext.h"

#include "../../WindowManager/WindowManager.h"

#include "../Extensions/Extensions.h"
#include "../ValidationLayers/ValidationLayers.h"
#include "../Instance/Instance.h"
#include "../Surface/Surface.h"
#include "../DebugMessenger/DebugMessenger.h"
#include "../Device/Physical/PhysicalDevice.h"
#include "../Device/Logical/Device.h"
#include "../QueueFamilies/QueueFamilies.h"

using namespace ascen;

VulkanContext::VulkanContext(WindowManager& windowManager) :
	mWindowManager(windowManager),
	mCommandPoolFactory(VK_NULL_HANDLE),
	mDescriptorFactory(VK_NULL_HANDLE),
	mSwapchainFactory(VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE),
	mGraphicsPipelineFactory(VK_NULL_HANDLE),
	mComputePipelineFactory(VK_NULL_HANDLE),
	mBufferFactory(VK_NULL_HANDLE, VK_NULL_HANDLE),
	mTextureFactory(VK_NULL_HANDLE, VK_NULL_HANDLE),
	mSamplerFactory(VK_NULL_HANDLE, VK_NULL_HANDLE),
	mFrameBufferFactory(VK_NULL_HANDLE),
	mRenderPassFactory(VK_NULL_HANDLE),
	mRenderTargetFactory(VK_NULL_HANDLE)
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
	QueueFamily computeFamily{};
	QueueFamilies::updateQueueFamilies(mPhysicalDevice, mSurface, graphicsFamily, presentFamily, computeFamily);
	mGraphicsFamilyIndex = graphicsFamily.value();
	mPresentFamilyIndex = presentFamily.value();
	mComputeFamilyIndex = computeFamily.value();

	mDevice = Device::create(
		mPhysicalDevice,
		graphicsFamily.value(),
		presentFamily.value(),
		computeFamily.value());

	mGraphicsQueue = QueueFamilies::getDeviceQueue(mDevice, mGraphicsFamilyIndex);
	mPresentQueue = QueueFamilies::getDeviceQueue(mDevice, mPresentFamilyIndex);
	mComputeQueue = QueueFamilies::getDeviceQueue(mDevice, mComputeFamilyIndex);

	mCommandPoolFactory      = CommandPoolFactory(mDevice);
	mDescriptorFactory       = DescriptorFactory(mDevice);
	mSwapchainFactory        = SwapchainFactory(mPhysicalDevice, mDevice, mSurface);
	mGraphicsPipelineFactory = GraphicsPipelineFactory(mDevice);
	mComputePipelineFactory  = ComputePipelineFactory(mDevice);
	mPushConstantFactory     = PushConstantFactory();
	mBufferFactory           = BufferFactory(mPhysicalDevice, mDevice);
	mTextureFactory          = TextureFactory(mPhysicalDevice, mDevice);
	mSamplerFactory          = SamplerFactory(mPhysicalDevice, mDevice);
	mFrameBufferFactory      = FrameBufferFactory(mDevice);
	mRenderPassFactory       = RenderPassFactory(mDevice);
	mRenderTargetFactory     = RenderTargetFactory(mDevice);
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

uint32_t VulkanContext::getComputeFamilyIndex() const
{
	return mComputeFamilyIndex;
}

VkQueue VulkanContext::getGraphicsQueue() const
{
	return mGraphicsQueue;
}

VkQueue VulkanContext::getPresentQueue() const
{
	return mPresentQueue;
}

VkQueue VulkanContext::getComputeQueue() const
{
	return mComputeQueue;
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

const GraphicsPipelineFactory& VulkanContext::getGraphicsPipelineFactory() const
{
	return mGraphicsPipelineFactory;
}

const ComputePipelineFactory& VulkanContext::getComputePipelineFactory() const
{
	return mComputePipelineFactory;
}

const PushConstantFactory& VulkanContext::getPushConstantFactory() const
{
	return mPushConstantFactory;
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

const FrameBufferFactory& VulkanContext::getFrameBufferFactory() const
{
	return mFrameBufferFactory;
}

const RenderPassFactory& VulkanContext::getRenderPassFactory() const
{
	return mRenderPassFactory;
}

const RenderTargetFactory& VulkanContext::getRenderTargetFactory() const
{
	return mRenderTargetFactory;
}

void VulkanContext::waitIdle() const
{
	vkDeviceWaitIdle(mDevice);
}

void VulkanContext::cleanup() const
{
	Device::destroy(mDevice);
	Surface::destroy(mInstance, mSurface);
	DebugMessenger::destroy(mInstance, mDebugMessenger);
	Instance::destroy(mInstance);
}
