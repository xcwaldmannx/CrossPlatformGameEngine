#include "MyGraphicsPipeline.h"

#include "../Graphics/Extensions/Extensions.h"
#include "../Graphics/ValidationLayers/ValidationLayers.h"
#include "../Graphics/Instance/Instance.h"
#include "../Graphics/Surface/Surface.h"
#include "../Graphics/DebugMessenger/DebugMessenger.h"
#include "../Graphics/Device/Physical/PhysicalDevice.h"
#include "../Graphics/Device/Logical/Device.h"
#include "../Graphics/Descriptor/Descriptor.h"

#include "../Graphics/Vertex/TextureVertex.h"

MyGraphicsPipeline::MyGraphicsPipeline()
{
}

void MyGraphicsPipeline::create()
{
	mWindowManager.init();
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
	mSurface = ascen::Surface::create(mInstance, mWindowManager.getWindow());

	mDebugMessenger = ascen::DebugMessenger::create(mInstance);

	mPhysicalDevice = ascen::PhysicalDevice::get(mInstance, mSurface);

	ascen::QueueFamilies::updateQueueFamilies(mPhysicalDevice, mSurface, mGraphicsFamily, mPresentFamily);

	mDevice = ascen::Device::create(mPhysicalDevice, mGraphicsFamily.value(), mPresentFamily.value());

	mSwapchain = std::make_shared<ascen::Swapchain>(
		mWindowManager.getWindow(),
		mPhysicalDevice,
		mSurface,
		mGraphicsFamily.value(),
		mPresentFamily.value());
	mHandleManager.create(mDevice, mSwapchain.get());

	mRenderPass = std::make_shared<ascen::RenderPass>(
		mPhysicalDevice,
		mSwapchain->handle(),
		mRenderPass->handle());
	mHandleManager.create(mDevice, mRenderPass.get());

	// TODO: create depth texture

	mSwapchain->createFrameBuffers(mDevice, mRenderPass->handle(), 0/*put depth texture image view here*/);

	mCommandPool = std::make_shared<ascen::CommandPool>(mGraphicsFamily);
	mHandleManager.create(mDevice, mCommandPool.get());

	// TODO: create sync objects

	// TODO: load models and textures
	
	// TODO: create buffers
	createBuffers();


	mDescriptorPool = ascen::Descriptor::createPool(
		mDevice,
		{
			ascen::Descriptor::PoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1),
			ascen::Descriptor::PoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1),
			ascen::Descriptor::PoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1),
		}
	);

	mDescriptorSetLayout = ascen::Descriptor::createLayout(
		mDevice,
		{
			ascen::Descriptor::createBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT),
			ascen::Descriptor::createBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT),
			ascen::Descriptor::createBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
		}
	);

	mDescriptorSet = ascen::Descriptor::createSet(
		mDevice,
		mDescriptorPool,
		{ mDescriptorSetLayout }
	);

	auto writeUniformBuffer = ascen::Descriptor::createBufferWrite(
		mDescriptorSet,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
		nullptr,
		0,
		0,
		0);

	auto writeStorageBuffer = ascen::Descriptor::createBufferWrite(
		mDescriptorSet,
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
		nullptr,
		0,
		0,
		1);

	auto writeTextureBuffer = ascen::Descriptor::createImageWrite(
		mDescriptorSet,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
		nullptr,
		nullptr,
		2);

	ascen::Descriptor::updateSet(
		mDevice,
		mDescriptorSet,
		{
			writeUniformBuffer,
			writeStorageBuffer,
			writeTextureBuffer,
		}
	);

	mPipeline = std::make_shared<WireframePipeline>(
		"src/shaders/vert.spv",
		"src/shaders/frag.spv",
		mSwapchain->getExtent(),
		mDescriptorSetLayout,
		mRenderPass->handle());
	mHandleManager.create(mDevice, mPipeline.get());
}

void MyGraphicsPipeline::destroy()
{
	vkDeviceWaitIdle(mDevice);

	// TODO: destroy resources (textures, images, buffers, etc)

	// TODO: destroy sync objects
	
	mHandleManager.destroy(mDevice);

	ascen::Descriptor::destroy(mDevice, mDescriptorSetLayout, mDescriptorPool);

	//mPipeline->destroy(mDevice);
	//mCommandPool->destroy(mDevice);
	//mRenderPass->destroy(mDevice);
	//mSwapchain->destroy(mDevice);

	ascen::Device::destroy(mDevice);
	ascen::Surface::destroy(mInstance, mSurface);
	ascen::Instance::destroy(mInstance);

	mWindowManager.destroy();
}

void MyGraphicsPipeline::createDepthTexture()
{

}

void MyGraphicsPipeline::createBuffers()
{
	// TODO: load buffers with appropriate data

	mUniformBuffer = std::make_shared<ascen::Buffer>(
		ascen::Buffer::createUniformBuffer<int>(
			mPhysicalDevice,
			mDevice,
			2
		)
	);

	VkQueue graphicsQueue = ascen::QueueFamilies::getDeviceQueue(mDevice, mGraphicsFamily.value());

	mVertexBuffer = std::make_shared<ascen::Buffer>(
		ascen::Buffer::createVertexBuffer<ascen::TextureVertex>(
			mPhysicalDevice,
			mDevice,
			graphicsQueue,
			mCommandPool,
			{}
		)
	);

	mIndexBuffer = std::make_shared<ascen::Buffer>(
		ascen::Buffer::createIndexBuffer<uint32_t>(
			mPhysicalDevice,
			mDevice,
			graphicsQueue,
			mCommandPool,
			{}
		)
	);

	mStorageBuffer = std::make_shared<ascen::Buffer>(
		ascen::Buffer::createStorageBuffer<int>(
			mPhysicalDevice,
			mDevice,
			graphicsQueue,
			mCommandPool,
			{}
		)
	);
}
