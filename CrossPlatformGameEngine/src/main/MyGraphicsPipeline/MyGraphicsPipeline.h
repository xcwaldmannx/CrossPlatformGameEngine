#pragma once

#include "../Utility/ImageLoader/ImageLoader.h"

#include "../WindowManager/WindowManager.h"
#include "../Graphics/QueueFamilies/QueueFamilies.h"

#include "../Graphics/Swapchain/Swapchain.h"
#include "../Graphics/RenderPass/RenderPass.h"
#include "../Graphics/CommandPool/CommandPool.h"
#include "../Graphics/Pipeline/Pipeline.h"

#include "../Graphics/Resource/Buffer/Buffer.h"
#include "../Graphics/Resource/Image/Image.h"
#include "../Graphics/Resource/Texture/Texture.h"
#include "../Graphics/Resource/Sampler/Sampler.h"

#include "../Graphics/Vertex/TextureVertex.h"

#include <Mass.h>

#include <memory>

#include <stack>
#include <unordered_map>
#include <vector>

#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/* BEGIN
* Temporary structs to demonstrate GPU-driven rendering
*/

// model data, stored in gpu
struct GPUModelData
{
	uint32_t mVertexOffset = 0;
	uint32_t mVertexCount  = 0;
	uint32_t mIndexOffset  = 0;
	uint32_t mIndexCount   = 0;
};

// per-instance data for an entity, stored in gpu
struct GPUInstanceData
{
	glm::mat4 mTransform = glm::mat4(1.0);
	uint32_t mModelId = 0;
};

/* END
* Temporary structs to demonstrate GPU-driven rendering
*/

struct alignas(16) UBOStruct
{
	glm::mat4 mView;
	glm::mat4 mProj;
};

struct ModelInfo
{
	uint32_t mModelId   = 0;
	uint32_t mMeshCount = 0;

	std::vector<uint32_t> mVertexOffsets;
	std::vector<uint32_t> mIndexCounts;
	std::vector<uint32_t> mIndexOffsets;
	std::vector<uint32_t> mTransformOffsets;
};

class MyGraphicsPipeline
{
public:
	MyGraphicsPipeline(
		const WindowManager& windowManager,
		const std::unordered_map<uint32_t, ModelInfo>& data,
		const std::vector<float>& vertices,
		const std::vector<uint32_t>& indices,
		const std::vector<float>& transforms);

	void create();
	void destroy();

	void submit();

	void drawFrame();

	void resize();
	bool isResized() const;

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
	const WindowManager mWindowManager;
	bool mIsWindowResized = false;

	std::vector<const char*> mExtensions;
	std::vector<const char*> mValidationLayers;
	VkDebugUtilsMessengerEXT mDebugMessenger{};

	VkInstance mInstance = VK_NULL_HANDLE;
	VkSurfaceKHR mSurface = VK_NULL_HANDLE;

	VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
	ascen::QueueFamily mGraphicsFamily;
	ascen::QueueFamily mPresentFamily;

	VkDevice mDevice = VK_NULL_HANDLE;

	VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorSet mDescriptorSet = VK_NULL_HANDLE;

	std::shared_ptr<ascen::Swapchain> mSwapchain = nullptr;
	std::shared_ptr<ascen::RenderPass> mRenderPass = nullptr;
	std::shared_ptr<ascen::Pipeline<ascen::TextureVertex>> mPipeline = nullptr;
	std::shared_ptr<ascen::CommandPool> mCommandPool = nullptr;

	std::vector<VkSemaphore> mImageAvailableSemaphores;
	std::vector<VkSemaphore> mRenderFinishedSemaphores;
	std::vector<VkFence> mInFlightFences;

	uint32_t MAX_FRAMES_IN_FLIGHT = 2;
	uint32_t mCurrentFrame = 0;

	std::shared_ptr<ascen::Buffer> mUniformBuffer = nullptr;
	std::shared_ptr<ascen::Buffer> mVertexBuffer = nullptr;
	std::shared_ptr<ascen::Buffer> mIndexBuffer = nullptr;

	std::shared_ptr<ascen::Buffer> mSBOModelData = nullptr;
	std::shared_ptr<ascen::Buffer> mSBOInstanceData = nullptr;

	std::shared_ptr<ascen::Texture> mDepthTexture = nullptr;

	std::shared_ptr<ascen::Texture> mTexture = nullptr;
	std::shared_ptr<ascen::Sampler> mSampler = nullptr;

	const std::unordered_map<uint32_t, ModelInfo> mModelInfo;
	const std::vector<float> mVertices;
	const std::vector<uint32_t> mIndices;
	const std::vector<float> mTransforms;

	ascen::CommandDrawData mCommandDrawData{};
};