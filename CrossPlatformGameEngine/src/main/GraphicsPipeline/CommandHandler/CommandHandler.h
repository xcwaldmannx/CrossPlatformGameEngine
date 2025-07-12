#pragma once

#include "../LogicalDeviceHandler/LogicalDeviceHandler.h"
#include "../SwapchainHandler/SwapchainHandler.h"
#include "../RenderPassHandler/RenderPassHandler.h"
#include "../PipelineHandler/PipelineHandler.h"

#include "../Entity.h"

#include <vector>

#include <vulkan/vulkan.h>


namespace ascen {

    struct Buffer;
    struct DescriptorGroup;
    struct Pipeline;
    struct RenderPass;

    struct CommandPool {
	    VkCommandPool mCommandPool = nullptr;
	    std::vector<VkCommandBuffer> mCommandBuffers;
    };

    struct DrawInfo {
        std::vector<Entity>* mEntities;
        ascen::Buffer* mVertexBuffer;
        ascen::Buffer* mIndexBuffer;
        //ascen::Buffer* mInstanceBuffer;
	    DescriptorGroup* mDescriptorGroup;
    };

    /**
    * @file CommandHandler.h
    *
    * @name createCommandPool
    * @brief [add a brief]
    *
    * @return void
    *
    * @param VkDevice logicalDevice
    * @param uint32_t graphicsFamily
    * @param CommandPool* commandpool
    */
    void createCommandPool(LogicalDevice& logicalDevice, uint32_t graphicsFamily, CommandPool* commandpool);

    /**
    * @file CommandHandler.h
    *
    * @name destroyCommandPool
    * @brief Destroys a command pool
    *
    * @return void
    *
    * @param VkDevice logicalDevice
    * @param CommandPool& commandpool
    */
    void destroyCommandPool(LogicalDevice& logicalDevice, CommandPool& commandpool);

    /**
    * @file CommandHandler.h
    *
    * @name recordCommandPool
    * @brief records draw commands
    *
    * @return void
    *
    * @param DrawInfo& drawInfo
    * @param uint32_t frameIndex
    * @param uint32_t imageIndex
    * @param Swapchain& swapchain
    * @param RenderPass& renderpass
    * @param Pipeline& pipeline
    * @param CommandPool& commandpool
    */
    void recordCommandPool(
        DrawInfo& drawInfo,
        uint32_t frameIndex,
        uint32_t imageIndex,
        Swapchain& swapchain,
        RenderPass& renderpass,
        Pipeline& pipeline,
        CommandPool& commandpool);

    /**
    * @file CommandHandler.h
    *
    * @name beginSingleTimeCommands
    * @brief [add a brief]
    *
    * @return void
    *
    * @param VkDevice logicalDevice
    * @param CommandPool& commandpool
    * @param VkCommandBuffer buffer
    */
    void beginSingleTimeCommands(
        LogicalDevice& logicalDevice,
        CommandPool& commandpool,
        VkCommandBuffer* buffer);

    /**
    * @file CommandHandler.h
    *
    * @name endSingleTimeCommands
    * @brief [add a brief]
    *
    * @return void
    *
    * @param VkDevice logicalDevice
    * @param VkQueue graphicsQueue
    * @param CommandPool& commandpool
    * @param VkCommandBuffer buffer
    */
    void endSingleTimeCommands(
        LogicalDevice& logicalDevice,
        VkQueue graphicsQueue,
        CommandPool& commandpool,
        VkCommandBuffer* buffer);

}