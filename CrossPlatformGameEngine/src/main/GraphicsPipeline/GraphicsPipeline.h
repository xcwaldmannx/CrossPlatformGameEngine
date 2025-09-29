#pragma once

#include "../WindowManager/WindowManager.h"
#include "VulkanInstance/VulkanInstance.h"
#include "SurfaceHandler/SurfaceHandler.h"
#include "PhysicalDeviceHandler/PhysicalDeviceHandler.h"
#include "QueueFamilyHandler/QueueFamilyHandler.h"
#include "LogicalDeviceHandler/LogicalDeviceHandler.h"
#include "DescriptorSetHandler/DescriptorSetHandler.h"
#include "SwapchainHandler/SwapchainHandler.h"
#include "RenderPassHandler/RenderPassHandler.h"
//#include "PipelineHandler/PipelineHandler.h"
#include "../Pipelines/WireframePipeline.h"
#include "CommandHandler/CommandHandler.h"

#include "Resource/ResourceCommonFunctions.h"
#include "Resource/BufferHandler/BufferHandler.h"
#include "Resource/ImageHandler/ImageHandler.h"
#include "Resource/TextureHandler/TextureHandler.h"

#include "../ModelManager/ModelManager.h"

#include "UniformBuffers.h"
#include "Entity.h"

#include "RenderComponents.h"

#include <iostream>

#include <optional>
#include <vector>
#include <map>
#include <unordered_map>

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif

#include <vulkan/vulkan.h>

struct alignas(16) PerEntityData {
	glm::mat4 mTransform;
	int mModelId;
	int mTextureId;
	int pad0[2];
};

/*
FILE: GraphicsPipeline.h
NAME: GraphicsPipeline
TYPE: CLASS
DESC: Instantiation of the Vulkan Graphics API
*/
class GraphicsPipeline {
public:
	void create();
	void destroy();

	void setModels(const ModelManager& modelManager);
	void setTextures(const std::vector<const char*>& filepaths);

	bool isRunning();
	void submit(std::vector<BasicRenderComponent>& renderComponents);
	void pollEvents();
	void render();

private:
	void loadModels();
	void loadTextures();

	void createSyncObjects();
	void destroySyncObjects();

	void createBuffers();
	void createDepthTexture();

	void updateUniformBuffer(uint32_t currentImage, ascen::Buffer& buffer);
	void updateStorageBuffer(ascen::Buffer& buffer, std::vector<PerEntityData>& data);

	void drawFrame();

private:
	WindowManager mWindowManager;
	ascen::VulkanInstance mInstance;

	ascen::Surface mSurface;

	ascen::PhysicalDevice mPhysicalDevice;

	std::optional<uint32_t> mGraphicsFamily;
	std::optional<uint32_t> mPresentFamily;

	ascen::LogicalDevice mLogicalDevice;

	ascen::DescriptorGroup mDescriptorGroup;
	std::vector<VkDescriptorSetLayoutBinding> mBindings;

	ascen::Swapchain mSwapchain;

	ascen::RenderPass mRenderPass;

	std::shared_ptr<ascen::Pipeline> mPipeline = nullptr;

	ascen::CommandPool mCommandPool;

	ascen::BufferCreateInfo mCreateBufferInfo;

	std::vector<VkSemaphore> mImageAvailableSemaphores;
	std::vector<VkSemaphore> mRenderFinishedSemaphores;
	std::vector<VkFence> mInFlightFences;

	static const int MAX_FRAMES_IN_FLIGHT = 2;
	uint32_t mCurrentFrame = 0;

	ascen::Texture mDepthTexture;

	ascen::Buffer mUniformBuffer;

	std::vector<PerEntityData> mEntityData;
	ascen::Buffer mStorageBuffer;

	ascen::Buffer mVertexBuffer;
	ascen::Buffer mIndexBuffer;
	ascen::Buffer mInstanceBuffer;
	ascen::TextureArray mTextureArray;

	ModelManager mModelManager;
	std::unordered_map<std::string, size_t> mModelOffets;
	std::map<int, int> mModelIdToCount;

	std::vector<const char*> mTextureFilepaths;

	// per material data
	//std::vector<ascen::Texture> mDiffuseTextures;
	//std::vector<ascen::Texture> mNormalTextures;
	//std::vector<ascen::Texture> mEmissionTextures;
};
