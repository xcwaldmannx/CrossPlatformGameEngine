#include "MyGraphicsPipeline.h"

#include "../Graphics/Extensions/Extensions.h"
#include "../Graphics/ValidationLayers/ValidationLayers.h"
#include "../Graphics/Instance/Instance.h"
#include "../Graphics/Surface/Surface.h"
#include "../Graphics/DebugMessenger/DebugMessenger.h"
#include "../Graphics/Device/Physical/PhysicalDevice.h"
#include "../Graphics/Device/Logical/Device.h"
#include "../Graphics/Descriptor/Descriptor.h"

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
		VK_FORMAT_R8G8B8A8_SRGB,
		ascen::PhysicalDevice::findDepthFormat(mPhysicalDevice));
	mHandleManager.create(mDevice, mRenderPass.get());

	mCommandPool = std::make_shared<ascen::CommandPool>(mGraphicsFamily.value());
	mHandleManager.create(mDevice, mCommandPool.get());

	createSyncObjects();

	// create depth texture and create frame buffers
	createDepthTexture();
	mSwapchain->createFrameBuffers(mDevice, mRenderPass->handle(), mDepthTexture->getView());

	// TODO: load models and textures
	createTexturesAndSamplers();
	
	// TODO: create buffers
	createBuffers();

	std::vector<ascen::Descriptor::PoolSize> poolSizes =
	{
		ascen::Descriptor::PoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1),
		ascen::Descriptor::PoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1),
		ascen::Descriptor::PoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1),
	};

	mDescriptorPool = ascen::Descriptor::createPool(mDevice, poolSizes);

	std::vector<VkDescriptorSetLayoutBinding> bindings =
	{
		ascen::Descriptor::createBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT),
		ascen::Descriptor::createBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT),
		ascen::Descriptor::createBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
	};

	mDescriptorSetLayout = ascen::Descriptor::createLayout(mDevice, bindings);

	std::vector<VkDescriptorSetLayout> layouts =
	{
		mDescriptorSetLayout
	};

	mDescriptorSet = ascen::Descriptor::createSet(mDevice, mDescriptorPool, layouts);

	auto writeUniformBuffer = ascen::Descriptor::createBufferWrite(
		mDescriptorSet,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
		mUniformBuffer->getBuffer(),
		0,
		sizeof(UBOStruct),
		0);

	auto writeStorageBuffer = ascen::Descriptor::createBufferWrite(
		mDescriptorSet,
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
		mStorageBuffer->getBuffer(),
		0,
		sizeof(SBOStruct),
		1);

	auto writeTextureBuffer = ascen::Descriptor::createImageWrite(
		mDescriptorSet,
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		mTexture->getView(),
		mSampler->getSampler(),
		2);

	std::vector<ascen::DescriptorWrite> writes = 
	{
		writeUniformBuffer,
		writeStorageBuffer,
		writeTextureBuffer,
	};

	ascen::Descriptor::updateSet(mDevice, writes);

	mPipeline = std::make_shared<WireframePipeline<ascen::TextureVertex>>(
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

	ascen::Buffer::destroy(mDevice, *mUniformBuffer);
	ascen::Buffer::destroy(mDevice, *mVertexBuffer);
	ascen::Buffer::destroy(mDevice, *mIndexBuffer);
	ascen::Buffer::destroy(mDevice, *mStorageBuffer);

	ascen::Texture::destroy(mDevice, *mTexture);
	ascen::Sampler::destroy(mDevice, *mSampler);
	ascen::Texture::destroy(mDevice, *mDepthTexture);

	destroySyncObjects();
	
	mHandleManager.destroy(mDevice);

	ascen::Descriptor::destroy(mDevice, mDescriptorSetLayout, mDescriptorPool);

	//mPipeline->destroy(mDevice);
	//mCommandPool->destroy(mDevice);
	//mRenderPass->destroy(mDevice);
	//mSwapchain->destroy(mDevice);

	ascen::Device::destroy(mDevice);
	ascen::Surface::destroy(mInstance, mSurface);
	ascen::DebugMessenger::destroy(mInstance, mDebugMessenger);
	ascen::Instance::destroy(mInstance);

	mWindowManager.destroy();
}

// TODO: finish drawFrame

void MyGraphicsPipeline::drawFrame()
{
	vkWaitForFences(mDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult nextImageResult = vkAcquireNextImageKHR(
		mDevice,
		mSwapchain->handle(),
		UINT64_MAX,
		mImageAvailableSemaphores[mCurrentFrame],
		VK_NULL_HANDLE,
		&imageIndex);

	if (nextImageResult == VK_ERROR_OUT_OF_DATE_KHR)
	{
		mSwapchain->destroy(mDevice);
		mSwapchain.reset();
		mSwapchain = std::make_shared<ascen::Swapchain>(
			mWindowManager.getWindow(),
			mPhysicalDevice,
			mSurface,
			mGraphicsFamily.value(),
			mPresentFamily.value());

		mSwapchain->create(mDevice);

		// ascen::destroyDepthTexture(mLogicalDevice, mDepthTexture);
		// createDepthTexture();

		mSwapchain->createFrameBuffers(
			mDevice,
			mRenderPass->handle(),
			0 /* put depth texture image view here */);

		return;
	}
	else if (nextImageResult != VK_SUCCESS && nextImageResult != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swapchain image!");
	}

	vkResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);

	updateUBO(mCurrentFrame);
	updateSBO<SBOStruct>(*mStorageBuffer, mSBOData);

	// TODO: create draw indo struct or something

	//ascen::DrawInfo drawInfo{
	//	&mVertexBuffer,
	//	&mIndexBuffer,
	//	&mModelManager,
	//	&mModelIdToCount,
	//	&mDescriptorGroup,
	//};

	// TODO: pass draw info into record command

	mCommandPool->record(
		mCurrentFrame,
		imageIndex,
		mDescriptorSet,
		mVertexBuffer->getBuffer(),
		mIndexBuffer->getBuffer(),
		mRenderPass,
		mSwapchain,
		mPipeline);

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
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	VkQueue presentQueue = ascen::QueueFamilies::getDeviceQueue(mDevice, mPresentFamily.value());
	VkResult queuePresentResult = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR)
	{
		mSwapchain->destroy(mDevice);
		mSwapchain.reset();
		mSwapchain = std::make_shared<ascen::Swapchain>(
			mWindowManager.getWindow(),
			mPhysicalDevice,
			mSurface,
			mGraphicsFamily.value(),
			mPresentFamily.value());

		mSwapchain->create(mDevice);

		// ascen::destroyDepthTexture(mLogicalDevice, mDepthTexture);
		// createDepthTexture();

		mSwapchain->createFrameBuffers(
			mDevice,
			mRenderPass->handle(),
			0 /* put depth image view here */);
	}
	else if (queuePresentResult != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void MyGraphicsPipeline::createSyncObjects()
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

void MyGraphicsPipeline::destroySyncObjects()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(mDevice, mRenderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(mDevice, mImageAvailableSemaphores[i], nullptr);
		vkDestroyFence(mDevice, mInFlightFences[i], nullptr);
	}
}

void MyGraphicsPipeline::createDepthTexture()
{
	mDepthTexture = std::make_shared<ascen::Texture>(
		ascen::Texture::create(
			mPhysicalDevice,
			mDevice,
			ascen::QueueFamilies::getDeviceQueue(mDevice, mGraphicsFamily.value()),
			mCommandPool,
			ascen::PhysicalDevice::findDepthFormat(mPhysicalDevice),
			VK_IMAGE_TILING_OPTIMAL,
			nullptr,
			mSwapchain->getExtent().width,
			mSwapchain->getExtent().height,
			1,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VK_IMAGE_ASPECT_DEPTH_BIT
		)
	);
}

void MyGraphicsPipeline::createBuffers()
{
	// TODO: load buffers with appropriate data
	mass::Configuration config{};
	config.mVertexLayout.mAttributes =
	{
		{ 3, sizeof(float), 0 },
		{ 3, sizeof(float), sizeof(float) * 3 },
		{ 2, sizeof(float), sizeof(float) * 6 },
	};
	config.mHasNormals    = true;
	config.mHasTexCoords  = true;
	config.mHasTransforms = true;

	mModelLayout = mass::deserialize(config, "res/testmodel.model");

	mUniformBuffer = std::make_shared<ascen::Buffer>(
		ascen::Buffer::createUniformBuffer<UBOStruct>(
			mPhysicalDevice,
			mDevice,
			2
		)
	);

	VkQueue graphicsQueue = ascen::QueueFamilies::getDeviceQueue(mDevice, mGraphicsFamily.value());

	mVertexBuffer = std::make_shared<ascen::Buffer>(
		ascen::Buffer::createVertexBuffer(
			mPhysicalDevice,
			mDevice,
			graphicsQueue,
			mCommandPool,
			mModelLayout.mVertices
		)
	);

	mIndexBuffer = std::make_shared<ascen::Buffer>(
		ascen::Buffer::createIndexBuffer<uint32_t>(
			mPhysicalDevice,
			mDevice,
			graphicsQueue,
			mCommandPool,
			mModelLayout.mIndices
		)
	);

	SBOStruct sbo{};
	sbo.mTransform = glm::mat4(0.0);

	mStorageBuffer = std::make_shared<ascen::Buffer>(
		ascen::Buffer::createStorageBuffer<SBOStruct>(
			mPhysicalDevice,
			mDevice,
			graphicsQueue,
			mCommandPool,
			{ sbo }
		)
	);
}

void MyGraphicsPipeline::createTexturesAndSamplers()
{
	ImageLoader il;
	RawImage raw;
	il.loadImage("res/testimg1.png", &raw);

	mTexture = std::make_shared<ascen::Texture>(
		ascen::Texture::create(
			mPhysicalDevice,
			mDevice,
			ascen::QueueFamilies::getDeviceQueue(mDevice, mGraphicsFamily.value()),
			mCommandPool,
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_TILING_OPTIMAL,
			raw.mPixels,
			raw.mWidth,
			raw.mHeight,
			1,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT
		)
	);

	mSampler = std::make_shared<ascen::Sampler>(
		ascen::Sampler::create(
			mPhysicalDevice,
			mDevice
		)
	);
}

void MyGraphicsPipeline::updateUBO(uint32_t imageIndex)
{
	UBOStruct ubo{};
	ubo.mView = glm::lookAt(glm::vec3(0.0f, -16.0f, 6.0f), glm::vec3(0.0f, 0.0f, 6.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.mProj = glm::perspective(glm::radians(70.0f),
		mSwapchain->getExtent().width / (float)mSwapchain->getExtent().height, 0.01f, 100.0f);
	ubo.mProj[1][1] *= -1;

	uint8_t offset = imageIndex * sizeof(UBOStruct);
	uint8_t* target = reinterpret_cast<uint8_t*>(mUniformBuffer->getBuffer());
	memcpy(target + offset, &ubo, sizeof(ubo));
}
