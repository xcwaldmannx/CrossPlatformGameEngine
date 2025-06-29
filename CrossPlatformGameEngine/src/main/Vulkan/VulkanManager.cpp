#include "VulkanManager.h"

#include <iostream>
#include <stdexcept>

VulkanManager::VulkanManager() {}

void VulkanManager::init() {
    mWindowManager.create();

	createInstance();

    if (mDebugManager.create(mVkInstance) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }

    mSurfaceManager.create(mVkInstance, mWindowManager.getWindow());
    const auto& surface = mSurfaceManager.getSurface();

    mPhysicalDeviceManager.establishPhysicalDevice(mVkInstance, surface);
    const auto& physicalDevice = mPhysicalDeviceManager.getDevice();

    mQueueFamilyManager.init(physicalDevice, surface);
    mQueueFamilyManager.updateQueueFamilies();

    mLogicalDeviceManager.create(physicalDevice, mQueueFamilyManager);
    const auto& logicalDevice = mLogicalDeviceManager.getDevice();

    // create descriptor sets
    mDescriptorSetLayout.createDescriptorSet(0, vulkan::UNIFORM_BUFFER, vulkan::VERTEX_SHADER);
    mDescriptorSetLayout.init(logicalDevice);

    mSwapchainManager.create(mWindowManager.getWindow(), mQueueFamilyManager, physicalDevice, logicalDevice, surface);

    mRenderPassManager.create(mSwapchainManager, logicalDevice);

    mGraphicsPipelineManager.create(logicalDevice, mSwapchainManager, mRenderPassManager, mShaderManager, mDescriptorSetLayout);

    mSwapchainManager.createFrameBuffers(logicalDevice, mRenderPassManager);

    mCommandManager.create(logicalDevice, mQueueFamilyManager);

    createSyncObjects();

    // create buffers and mesh

    mResourceManager.init(
        mPhysicalDeviceManager.getDevice(),
        mLogicalDeviceManager.getDevice(),
        &mCommandManager,
        &mQueueFamilyManager
    );

    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };

    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };

    mResourceManager.createVertexBuffer<Vertex>(mVertexBuffer, vertices);
    mResourceManager.createIndexBuffer<uint16_t>(mIndexBuffer, indices);
    mMesh.init(mVertexBuffer, mIndexBuffer);

    mCommandManager.addMesh(mMesh);

    // create uniform buffers
    // mResourceManager.createUniformBuffer<>();
}

void VulkanManager::mainLoop() {
    while (!glfwWindowShouldClose(mWindowManager.getWindow())) {
        glfwPollEvents();
        drawFrame();
    }

    vkDeviceWaitIdle(mLogicalDeviceManager.getDevice());
}

void VulkanManager::destroy() {
    const auto& logicalDevice = mLogicalDeviceManager.getDevice();

    // destroy vertex buffer
    mResourceManager.destroyBuffer(mVertexBuffer);
    mResourceManager.destroyBuffer(mIndexBuffer);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(logicalDevice, mRenderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(logicalDevice, mImageAvailableSemaphores[i], nullptr);
        vkDestroyFence(logicalDevice, mInFlightFences[i], nullptr);
    }

    mCommandManager.destroy(logicalDevice);

    mGraphicsPipelineManager.destroy(logicalDevice);

    mRenderPassManager.destroy(logicalDevice);

    mSwapchainManager.destroy(logicalDevice);

    mDescriptorSetLayout.destroy();

    mLogicalDeviceManager.destroy();

    if (mValidationLayerManager.isEnabled()) {
        mDebugManager.destroy(mVkInstance);
    }

    mSurfaceManager.destroy(mVkInstance);

    vkDestroyInstance(mVkInstance, nullptr);

    mWindowManager.destroy();
}

void VulkanManager::createInstance() {

    mValidationLayerManager.validateSupport();

    if (mValidationLayerManager.isEnabled()) {
        mExtensionManager.addExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    mExtensionManager.validateSupport();

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = mExtensionManager.getExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (mValidationLayerManager.isEnabled()) {
        const auto& validationLayers = mValidationLayerManager.getValidationLayers();
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        mDebugManager.setCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &mVkInstance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create VK instance!");
    }
}

void VulkanManager::createSyncObjects() {
    mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    mRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    const auto& logicalDevice = mLogicalDeviceManager.getDevice();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(logicalDevice, &fenceInfo, nullptr, &mInFlightFences[i]) != VK_SUCCESS) {

            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

void VulkanManager::drawFrame() {
    const auto& surface = mSurfaceManager.getSurface();
    const auto& logicalDevice = mLogicalDeviceManager.getDevice();
    const auto& physicalDevice = mPhysicalDeviceManager.getDevice();

    vkWaitForFences(mLogicalDeviceManager.getDevice(), 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult nextImageResult = vkAcquireNextImageKHR(mLogicalDeviceManager.getDevice(), mSwapchainManager.getSwapchain(), UINT64_MAX, mImageAvailableSemaphores[mCurrentFrame], VK_NULL_HANDLE, &imageIndex);

    if (nextImageResult == VK_ERROR_OUT_OF_DATE_KHR) {
        mSwapchainManager.recreate(mWindowManager.getWindow(), mQueueFamilyManager, physicalDevice, logicalDevice, surface, mRenderPassManager);
        return;
    }
    else if (nextImageResult != VK_SUCCESS && nextImageResult != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    vkResetFences(mLogicalDeviceManager.getDevice(), 1, &mInFlightFences[mCurrentFrame]);

    mCommandManager.record(mCurrentFrame, imageIndex, mRenderPassManager, mSwapchainManager, mGraphicsPipelineManager);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mCommandManager.getCommandBuffer(mCurrentFrame);

    VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    uint32_t graphicsFamilyIndex = mQueueFamilyManager.getGraphicsFamilyIndex();
    const auto& graphicsQueue = mQueueFamilyManager.getDeviceQueue(mLogicalDeviceManager.getDevice(), graphicsFamilyIndex);

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, mInFlightFences[mCurrentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { mSwapchainManager.getSwapchain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    uint32_t presentFamilyIndex = mQueueFamilyManager.getPresentFamilyIndex();

    const auto& presentQueue = mQueueFamilyManager.getDeviceQueue(mLogicalDeviceManager.getDevice(), presentFamilyIndex);

    VkResult queuePresentResult = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR) {
        mSwapchainManager.recreate(mWindowManager.getWindow(), mQueueFamilyManager, physicalDevice, logicalDevice, surface, mRenderPassManager);
    }
    else if (queuePresentResult != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
