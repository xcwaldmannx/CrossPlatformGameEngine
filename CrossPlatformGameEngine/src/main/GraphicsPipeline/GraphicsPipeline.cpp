#include "GraphicsPipeline.h"

#include "../Utility/FileIO/FileIO.h"
#include "ShaderHandler/ShaderHandler.h"

#include "Vertex/Vertex.h"

#include <stdexcept>
#include <chrono>

/*
Begin GraphicsPipeline Implementation
*/

void GraphicsPipeline::create()
{
	// Create a GLFW window to display graphics
	mWindowManager.create();

	// Create the vulkan instance
	ascen::createVulkanInstance(&mInstance);

	// Create a surface between Vulkan and GLFW
	ascen::createSurface(mInstance, mWindowManager.getWindow(), &mSurface);

	// Pick a physical device based on the surface
	ascen::pickPhysicalDevice(mInstance, mSurface, &mPhysicalDevice);

	// Find queue families
	ascen::updateQueueFamilies(mPhysicalDevice, mSurface, mGraphicsFamily, mPresentFamily);

	// Create a logical device
	ascen::createLogicalDevice(
		mPhysicalDevice,
		mGraphicsFamily.value(), mPresentFamily.value(),
		&mLogicalDevice);

	// create a swapchain
	ascen::createSwapchain(
		mWindowManager.getWindow(),
		mPhysicalDevice,
		mLogicalDevice,
		mSurface,
		mGraphicsFamily.value(),
		mPresentFamily.value(),
		mSwapchain);

	// create a renderpass
	ascen::createRenderPass(mPhysicalDevice, mLogicalDevice, mSwapchain, mRenderPass);

	// create the depth buffer/texture
	createDepthTexture();

	// create frame buffers
	ascen::createFrameBuffers(mLogicalDevice, mSwapchain, mRenderPass, mDepthTexture);

	// create command pool
	ascen::createCommandPool(mLogicalDevice, mGraphicsFamily.value(), &mCommandPool);

	// create sync objects
	createSyncObjects();

	// create buffers and textures
	createBuffers();
	createTextures();

	// Create descriptor sets
	VkDescriptorSetLayoutBinding binding0;
	ascen::createDescriptorSetLayoutBinding(0, ascen::UNIFORM_BUFFER_DYNAMIC, ascen::VERTEX_SHADER, &binding0);
	mBindings.emplace_back(std::move(binding0));
	VkDescriptorSetLayoutBinding binding1;
	ascen::createDescriptorSetLayoutBinding(1, ascen::STORAGE_BUFFER_DYNAMIC, ascen::PIXEL_SHADER, &binding1);
	mBindings.emplace_back(std::move(binding1));
	VkDescriptorSetLayoutBinding binding2;
	ascen::createDescriptorSetLayoutBinding(2, ascen::TEXTURE_SAMPLER, ascen::PIXEL_SHADER, &binding2);
	mBindings.emplace_back(std::move(binding2));

	// create descriptor group
	ascen::DescriptorGroupCreateInfo descriptorGroupInfo{};
	descriptorGroupInfo.logicalDevice = &mLogicalDevice;
	descriptorGroupInfo.bindings = &mBindings;
	descriptorGroupInfo.mUniformBuffer = &mUniformBuffer;
	descriptorGroupInfo.mStorageBuffer = &mStorageBuffer;
	descriptorGroupInfo.mTextureArray = &mTextureArray;

	ascen::createDescriptorGroup(descriptorGroupInfo, &mDescriptorGroup);

	// create shaders
	auto vertShaderCode = FileIO::readFile("src/shaders/vert.spv");
	auto fragShaderCode = FileIO::readFile("src/shaders/frag.spv");

	ShaderModule vertShader;
	ShaderModule fragShader;
	ascen::createShader(mLogicalDevice, vertShaderCode, &vertShader);
	ascen::createShader(mLogicalDevice, fragShaderCode, &fragShader);

	// create pipeline
	ascen::createPipeline(
		mLogicalDevice,
		mSwapchain,
		vertShader,
		fragShader,
		mDescriptorGroup,
		mRenderPass,
		&mPipeline);

	// destroy shaders after creating the pipeline
	ascen::destroyShader(mLogicalDevice, vertShader);
	ascen::destroyShader(mLogicalDevice, fragShader);

	// create some entities
	Entity e1;
	e1.mModel = mModelManager.getModel("model_03");
	e1.mTextureIndex = 1;

	mEntities.emplace_back(std::move(e1));
}

void GraphicsPipeline::createBuffers()
{
	// setup create buffer info to easily create buffers
	mCreateBufferInfo.mPhysicalDevice = &mPhysicalDevice;
	mCreateBufferInfo.mLogicalDevice = &mLogicalDevice;
	mCreateBufferInfo.mGraphicsQueue = ascen::getDeviceQueue(mLogicalDevice, mGraphicsFamily.value());
	mCreateBufferInfo.mCommandPool = &mCommandPool;

	// create uniform buffer with enough memory for each frame in flight
	ascen::createUniformBuffer<UniformBufferObject>(mCreateBufferInfo, MAX_FRAMES_IN_FLIGHT, mUniformBuffer);

	// create vertex buffer containing all vertex data

	mModelManager.createModel("model_01", "C:\\Users\\xcwal\\Documents\\Models\\testmodel.fbx");
	mModelManager.createModel("model_02", "C:\\Users\\xcwal\\Documents\\Models\\testmodel_1.fbx");
	mModelManager.createModel("model_03", "C:\\Users\\xcwal\\Documents\\Models\\testmodel_2.fbx");

	ascen::createVertexBuffer<Vertex>(mCreateBufferInfo, mVertexBuffer, mModelManager.getVertices());
	ascen::createIndexBuffer<uint32_t>(mCreateBufferInfo, mIndexBuffer, mModelManager.getIndices());

	// create SSBO containing all entity data
	mEntityData.emplace_back(0, 2, 0, 0);
	ascen::createStorageBuffer<PerEntityData>(mCreateBufferInfo, mEntityData, mStorageBuffer);
}

void GraphicsPipeline::createTextures() 
{
	ascen::ImageCreateInfo imgInfo{};
	imgInfo.mPhysicalDevice = &mPhysicalDevice;
	imgInfo.mLogicalDevice = &mLogicalDevice;
	imgInfo.mCommandPool = &mCommandPool;
	imgInfo.mGraphicsQueue = ascen::getDeviceQueue(mLogicalDevice, mGraphicsFamily.value());
	imgInfo.mUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imgInfo.mProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	imgInfo.mFormat = VK_FORMAT_R8G8B8A8_SRGB;
	imgInfo.mTiling = VK_IMAGE_TILING_OPTIMAL;
	imgInfo.mFlags = VK_IMAGE_ASPECT_COLOR_BIT;

	ascen::TextureArrayCreateInfo texInfo{};
	texInfo.mImageCreateInfo = &imgInfo;
	texInfo.mSampler; // = someSampler;

	std::vector<const char*> filepaths =
	{
		//"C:\\Users\\xcwal\\OneDrive\\Desktop\\testimg1.png",
		//"C:\\Users\\xcwal\\OneDrive\\Desktop\\testimg2.png",
		"C:\\Users\\xcwal\\Documents\\Models\\Ernest_body_base color.png",
	};

	std::vector<RawImage> rawImages;
	rawImages.resize(filepaths.size());
	for (size_t i = 0; i < filepaths.size(); i++)
	{
		ImageLoader::loadImage(filepaths[i], &rawImages[i]);
	}

	ascen::createTextureArray(texInfo, rawImages, &mTextureArray);
}

void GraphicsPipeline::createDepthTexture() 
{
	ascen::ImageCreateInfo imgInfo{};
	imgInfo.mPhysicalDevice = &mPhysicalDevice;
	imgInfo.mLogicalDevice = &mLogicalDevice;
	imgInfo.mCommandPool = &mCommandPool;
	imgInfo.mGraphicsQueue = ascen::getDeviceQueue(mLogicalDevice, mGraphicsFamily.value());
	imgInfo.mUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imgInfo.mProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	imgInfo.mFormat = ascen::findDepthFormat(mPhysicalDevice);
	imgInfo.mTiling = VK_IMAGE_TILING_OPTIMAL;
	imgInfo.mFlags = VK_IMAGE_ASPECT_DEPTH_BIT;

	ascen::DepthImageCreateInfo depthImgInfo{};
	depthImgInfo.mImageCreateInfo = &imgInfo;
	depthImgInfo.mSwapchain = &mSwapchain;

	ascen::DepthTextureCreateInfo depthTexInfo{};
	depthTexInfo.mDepthImageCreateInfo = &depthImgInfo;

	ascen::createDepthTexture(depthTexInfo, &mDepthTexture);
}

void GraphicsPipeline::destroy()
{
	ascen::destroyTextureArray(mLogicalDevice, mTextureArray);

	ascen::destroyBuffer(mLogicalDevice, mStorageBuffer);

	ascen::destroyBuffer(mLogicalDevice, mUniformBuffer);

	ascen::destroyBuffer(mLogicalDevice, mIndexBuffer);

	ascen::destroyBuffer(mLogicalDevice, mVertexBuffer);

	// destroy the depth buffer/texture
	ascen::destroyDepthTexture(mLogicalDevice, mDepthTexture);

	destroySyncObjects();

	ascen::destroyCommandPool(mLogicalDevice, mCommandPool);
	ascen::destroyPipeline(mLogicalDevice, mPipeline);
	ascen::destroyRenderPass(mLogicalDevice, mRenderPass);
	ascen::destroySwapchain(mLogicalDevice, mSwapchain);
	ascen::destroyDescriptorGroup(mLogicalDevice, mDescriptorGroup);
	ascen::destroyLogicalDevice(mLogicalDevice);
	ascen::destroySurface(mInstance, mSurface);

	ascen::destroyVulkanInstance(mInstance);

	mWindowManager.destroy();
}

void GraphicsPipeline::renderLoop()
{
	while (!glfwWindowShouldClose(mWindowManager.getWindow()))
	{
		glfwPollEvents();
		drawFrame();
	}

	vkDeviceWaitIdle(mLogicalDevice.mDevice);
}

void GraphicsPipeline::createSyncObjects() {
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
		if (vkCreateSemaphore(mLogicalDevice.mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(mLogicalDevice.mDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(mLogicalDevice.mDevice, &fenceInfo, nullptr, &mInFlightFences[i]) != VK_SUCCESS)
		{

			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

void GraphicsPipeline::destroySyncObjects()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(mLogicalDevice.mDevice, mRenderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(mLogicalDevice.mDevice, mImageAvailableSemaphores[i], nullptr);
		vkDestroyFence(mLogicalDevice.mDevice, mInFlightFences[i], nullptr);
	}
}

void GraphicsPipeline::updateUniformBuffer(uint32_t currentImage, ascen::Buffer& buffer)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(3.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.projection = glm::perspective(glm::radians(45.0f),
		mSwapchain.mExtent.width / (float)mSwapchain.mExtent.height, 0.1f, 10.0f);
	ubo.projection[1][1] *= -1;

	uint8_t offset = currentImage * sizeof(UniformBufferObject);
	uint8_t* target = reinterpret_cast<uint8_t*>(buffer.mMappedMemory);
	memcpy(target + offset, &ubo, sizeof(ubo));

}

void GraphicsPipeline::updateStorageBuffer(ascen::Buffer& buffer, std::vector<PerEntityData>& data)
{

	data[0].textureIdx = 0;

	void* mappedData;
	vkMapMemory(mLogicalDevice.mDevice, buffer.mMemory, 0, buffer.mBufferSizeBytes, 0, &mappedData);
	std::memcpy(mappedData, data.data(), buffer.mBufferSizeBytes);
	vkUnmapMemory(mLogicalDevice.mDevice, buffer.mMemory);
}


void GraphicsPipeline::drawFrame()
{
	vkWaitForFences(mLogicalDevice.mDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult nextImageResult = vkAcquireNextImageKHR(
		mLogicalDevice.mDevice,
		mSwapchain.mSwapchain,
		UINT64_MAX,
		mImageAvailableSemaphores[mCurrentFrame],
		VK_NULL_HANDLE,
		&imageIndex);

	if (nextImageResult == VK_ERROR_OUT_OF_DATE_KHR)
	{
		ascen::recreateSwapchain(
			mWindowManager.getWindow(),
			mPhysicalDevice,
			mLogicalDevice,
			mSurface,
			mGraphicsFamily.value(),
			mPresentFamily.value(),
			mSwapchain);
		ascen::destroyDepthTexture(mLogicalDevice, mDepthTexture);
		createDepthTexture();
		ascen::createFrameBuffers(mLogicalDevice, mSwapchain, mRenderPass, mDepthTexture);
		return;
	}
	else if (nextImageResult != VK_SUCCESS && nextImageResult != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swapchain image!");
	}

	vkResetFences(mLogicalDevice.mDevice, 1, &mInFlightFences[mCurrentFrame]);

	updateUniformBuffer(mCurrentFrame, mUniformBuffer);
	// updateStorageBuffer(mStorageBuffer, mEntityData);

	ascen::DrawInfo drawInfo{
		&mEntities,
		&mVertexBuffer,
		&mIndexBuffer,
		&mDescriptorGroup,
	};

	ascen::recordCommandPool(drawInfo, mCurrentFrame, imageIndex, mSwapchain, mRenderPass, mPipeline, mCommandPool);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &mCommandPool.mCommandBuffers[mCurrentFrame];

	VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	const auto& graphicsQueue = ascen::getDeviceQueue(mLogicalDevice, mGraphicsFamily.value());

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, mInFlightFences[mCurrentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { mSwapchain.mSwapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	const auto& presentQueue = ascen::getDeviceQueue(mLogicalDevice, mPresentFamily.value());
	VkResult queuePresentResult = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR)
	{
		ascen::recreateSwapchain(
			mWindowManager.getWindow(),
			mPhysicalDevice,
			mLogicalDevice,
			mSurface,
			mGraphicsFamily.value(),
			mPresentFamily.value(),
			mSwapchain);
		ascen::destroyDepthTexture(mLogicalDevice, mDepthTexture);
		createDepthTexture();
		ascen::createFrameBuffers(mLogicalDevice, mSwapchain, mRenderPass, mDepthTexture);
	}
	else if (queuePresentResult != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

/*
End GraphicsPipeline Implementation
*/
