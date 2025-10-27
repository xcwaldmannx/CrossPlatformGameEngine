#pragma once

#include "../Utility/ImageLoader/ImageLoader.h"

#include "../WindowManager/WindowManager.h"
#include "../Graphics/QueueFamilies/QueueFamilies.h"

#include "../Graphics/HandleManager/HandleManager.h"
#include "../Graphics/Swapchain/Swapchain.h"
#include "../Graphics/RenderPass/RenderPass.h"
#include "../Graphics/CommandPool/CommandPool.h"

#include "../Graphics/Resource/Buffer/Buffer.h"
#include "../Graphics/Resource/Image/Image.h"
#include "../Graphics/Resource/Texture/Texture.h"
#include "../Graphics/Resource/Sampler/Sampler.h"

#include "../Graphics/Vertex/TextureVertex.h"
#include "../Graphics/Pipeline/WireframePipeline.h"

#include <Mass.h>

#include <memory>

#include <stack>
#include <vector>

#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct UBOStruct
{
	glm::mat4 mView;
	glm::mat4 mProj;
};

struct SBOStruct
{
	glm::mat4 mTransform;
	int modelId;
	int textureId;
};

class MyGraphicsPipeline
{
public:
	MyGraphicsPipeline();

	void create();
	void destroy();

	void drawFrame();

private:
	void createSyncObjects();
	void destroySyncObjects();

	void createDepthTexture();

	void createBuffers();
	void createTexturesAndSamplers();

	void updateUBO(uint32_t imageIndex);

	template<typename T>
	void updateSBO(ascen::Buffer& buffer, const std::vector<T>& data)
	{
		ascen::Buffer::updateStorageBuffer<T>(
			mPhysicalDevice,
			mDevice,
			ascen::QueueFamilies::getDeviceQueue(mDevice, mGraphicsFamily.value()),
			mCommandPool,
			buffer,
			data);
	}

private:
	WindowManager mWindowManager;

	std::vector<const char*> mExtensions;
	std::vector<const char*> mValidationLayers;
	VkDebugUtilsMessengerEXT mDebugMessenger{};

	VkInstance mInstance = VK_NULL_HANDLE;
	VkSurfaceKHR mSurface = VK_NULL_HANDLE;

	VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
	ascen::QueueFamily mGraphicsFamily;
	ascen::QueueFamily mPresentFamily;

	VkDevice mDevice = VK_NULL_HANDLE;

	ascen::HandleManager mHandleManager;

	VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorSet mDescriptorSet = VK_NULL_HANDLE;

	std::shared_ptr<ascen::Swapchain> mSwapchain = nullptr;
	std::shared_ptr<ascen::RenderPass> mRenderPass = nullptr;
	std::shared_ptr<ascen::Pipeline<ascen::TextureVertex>> mPipeline = nullptr;
	std::shared_ptr<ascen::CommandPool> mCommandPool = nullptr;

	mass::ModelLayout mModelLayout;

	std::shared_ptr<ascen::Buffer> mUniformBuffer = nullptr;
	std::shared_ptr<ascen::Buffer> mVertexBuffer = nullptr;
	std::shared_ptr<ascen::Buffer> mIndexBuffer = nullptr;
	std::shared_ptr<ascen::Buffer> mStorageBuffer = nullptr;

	std::shared_ptr<ascen::Texture> mDepthTexture = nullptr;

	std::shared_ptr<ascen::Texture> mTexture = nullptr;
	std::shared_ptr<ascen::Sampler> mSampler = nullptr;

	std::vector<VkSemaphore> mImageAvailableSemaphores;
	std::vector<VkSemaphore> mRenderFinishedSemaphores;
	std::vector<VkFence> mInFlightFences;

	uint32_t MAX_FRAMES_IN_FLIGHT = 2;
	uint32_t mCurrentFrame = 0;

	std::vector<SBOStruct> mSBOData =
	{
		{ glm::mat4(1) },
		{ glm::mat4(1) }
	};
};