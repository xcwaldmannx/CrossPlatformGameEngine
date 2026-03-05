#pragma once

#include "../GpuFramePass.h"

namespace ascen
{

    enum class GraphicsMode
    {
        MESH,
        LINES,
    };

    struct GraphicsGpuFramePass : GpuFramePass
    {
        GraphicsGpuFramePass(
            const FramePassType type,
            const GraphicsMode mode,
            const std::vector<VkDescriptorSet> descriptorSets,
            const VkPipeline pipeline,
            const VkPipelineLayout pipelineLayout,
            const std::vector<std::string> readBuffers,
            const std::vector<std::string> writeBuffers,
            const std::vector<VkImageView> readTextures,
            const std::vector<VkImageView> writeTextures,
            const std::vector<VkBuffer> vertexBuffers,
            const VkBuffer indexBuffer);

        const std::vector<VkBuffer> mVertexBuffers;
        const VkBuffer mIndexBuffer = VK_NULL_HANDLE;
        const GraphicsMode mMode;
    };

}