#include "MyPipeline.h"

#include "../Ecs/Systems/FrustumCullingSystem.h"

#include <glm/glm.hpp>

MyPipeline::MyPipeline(ascen::Engine& engine) : mEngine(engine) {}

void MyPipeline::init()
{
    initResources();
    initStages();
    initFramePasses();
}

void MyPipeline::initResources()
{
    // Vertices
    const ascen::VertexBinding vertexBindingTri{ 0, sizeof(float) * 8, VK_VERTEX_INPUT_RATE_VERTEX };
    const std::vector<ascen::VertexAttribute> vertexAttribTri =
    {
        { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 },
        { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3 },
        { 2, 0, VK_FORMAT_R32G32_SFLOAT, sizeof(float) * 6 }
    };

    mVertexTriangles = mEngine.registerResource<ascen::registry::VertexEntry>({ "VERTEX_TRIANGLES", vertexBindingTri, vertexAttribTri });

    const ascen::VertexBinding vertexBindingPoint{ 0, sizeof(float) * 4, VK_VERTEX_INPUT_RATE_VERTEX };
    const std::vector<ascen::VertexAttribute> vertexAttribPoint =
    {
        { 0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0 },
    };

    mVertexPoint = mEngine.registerResource<ascen::registry::VertexEntry>({ "VERTEX_POINT", vertexBindingPoint, vertexAttribPoint });

    // Buffers
    // mEngine.resource().registerBuffer({ "BUFFER_CAMERA", ascen::BufferType::UNIFORM, 2, sizeof(glm::mat4) * 2 });

    mBufferVertex = mEngine.registerResource<ascen::registry::BufferEntry>({ "BUFFER_VERTEX", 100000, sizeof(float) * 8,
        ascen::BUFFER_USAGE_VERTEX | ascen::BUFFER_USAGE_TRANSFER_DST, ascen::BUFFER_MEMORY_LOCAL });
    mBufferIndex = mEngine.registerResource<ascen::registry::BufferEntry>({ "BUFFER_INDEX", 100000, sizeof(uint32_t),
        ascen::BUFFER_USAGE_INDEX | ascen::BUFFER_USAGE_TRANSFER_DST, ascen::BUFFER_MEMORY_LOCAL });

    mBufferVertexBounds = mEngine.registerResource<ascen::registry::BufferEntry>({ "BUFFER_VERTEX_BOUNDS", 10'000, sizeof(float) * 4,
        ascen::BUFFER_USAGE_VERTEX | ascen::BUFFER_USAGE_STORAGE | ascen::BUFFER_USAGE_TRANSFER_SRC, ascen::BUFFER_MEMORY_LOCAL });
    mBufferIndexBounds = mEngine.registerResource<ascen::registry::BufferEntry>({ "BUFFER_INDEX_BOUNDS", 10'000 * 24, sizeof(uint32_t),
    ascen::BUFFER_USAGE_INDEX | ascen::BUFFER_USAGE_STORAGE | ascen::BUFFER_USAGE_TRANSFER_SRC, ascen::BUFFER_MEMORY_LOCAL });

    mBufferEntity = mEngine.registerResource<ascen::registry::BufferEntry>({ "BUFFER_ENTITY", 1'000'000, sizeof(FrustumCullingSystem::Entity),
        ascen::BUFFER_USAGE_STORAGE | ascen::BUFFER_USAGE_TRANSFER_DST, ascen::BUFFER_MEMORY_LOCAL });
    mBufferIndirect = mEngine.registerResource<ascen::registry::BufferEntry>({ "BUFFER_DRAWS", mBufferIndirectSize, sizeof(ascen::IndexedIndirectDraw),
        ascen::BUFFER_USAGE_INDIRECT | ascen::BUFFER_USAGE_STORAGE | ascen::BUFFER_USAGE_TRANSFER_DST, ascen::BUFFER_MEMORY_LOCAL });

    // Samplers
    mSampler = mEngine.registerResource<ascen::registry::SamplerEntry>({ "SAMPLER" });

    // Textures
    mTexture = mEngine.registerResource<ascen::registry::TextureEntry>({ "TEXTURE", ascen::TextureType::IMAGE, 1024, 1024, 1 });

    // RenderPass and RenderTarget
    ascen::renderpass::Attachment colorAttachment{};
    colorAttachment.mType = ascen::ATTACHMENT_PRESENT;
    colorAttachment.mFormat = ascen::FORMAT_RGBA8_SRGB;
    colorAttachment.mLoadOp = ascen::LOAD_OP_CLEAR;
    colorAttachment.mStoreOp = ascen::STORE_OP_STORE;
    colorAttachment.mDepthStencilLoadOp = ascen::LOAD_OP_NA;
    colorAttachment.mDepthStencilStoreOp = ascen::STORE_OP_NA;

    ascen::renderpass::Attachment depthAttachment{};
    depthAttachment.mType = ascen::ATTACHMENT_DEPTH;
    depthAttachment.mFormat = mEngine.getDepthFormat();
    depthAttachment.mLoadOp = ascen::LOAD_OP_CLEAR;
    depthAttachment.mStoreOp = ascen::STORE_OP_NA;
    depthAttachment.mDepthStencilLoadOp = ascen::LOAD_OP_NA;
    depthAttachment.mDepthStencilStoreOp = ascen::STORE_OP_NA;

    ascen::renderpass::SubPass subPass{};
    subPass.mBindPoint = ascen::BIND_POINT_GRAPHICS;
    subPass.mColorAttachmentIndices = { 0 };
    subPass.mDepthAttachmentIndex = 1;

    ascen::renderpass::SubPassDependency subPassDependency{};
    subPassDependency.mSrcSubpass = VK_SUBPASS_EXTERNAL;
    subPassDependency.mDstSubpass = 0;
    subPassDependency.mSrcAccessMask = ascen::ACCESS_NONE;
    subPassDependency.mDstAccessMask = ascen::ACCESS_COLOR_ATTACHMENT_WRITE | ascen::ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE;
    subPassDependency.mSrcStageMask = ascen::PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT | ascen::PIPELINE_STAGE_EARLY_FRAGMENT_TESTS;
    subPassDependency.mDstStageMask = ascen::PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT | ascen::PIPELINE_STAGE_EARLY_FRAGMENT_TESTS;

    ascen::registry::RenderPassEntry renderPassEntry{};
    renderPassEntry.mName = "RENDER_PASS";
    renderPassEntry.mAttachments = { colorAttachment, depthAttachment };
    renderPassEntry.mSubPasses = { subPass };
    renderPassEntry.mSubPassDependencies = { subPassDependency };

    mRenderPass = mEngine.registerResource<ascen::registry::RenderPassEntry>(renderPassEntry);

    ascen::registry::RenderTargetEntry renderTargetEntry{};
    renderTargetEntry.mName = "RENDER_TARGET";
    renderTargetEntry.mFormat = mEngine.getImageFormat();
    renderTargetEntry.mImages = mEngine.getPresentImages();
    renderTargetEntry.mImageCount = mEngine.getPresentImages().size();

    mRenderTarget = mEngine.registerResource<ascen::registry::RenderTargetEntry>(renderTargetEntry);

    // ascen::registry::FrameBufferEntry frameBufferEntry{};
    // frameBufferEntry.mName = "FRAMEBUFFER";
    // frameBufferEntry.mRenderPassId = mRenderPass;
    // frameBufferEntry.mRenderTargetId = mRenderTarget;
    // frameBufferEntry.mWidth = mEngine.getScreenWidth();
    // frameBufferEntry.mHeight = mEngine.getScreenHeight();
    //
    // mFrameBuffer = mEngine.registerResource<ascen::registry::FrameBufferEntry>(frameBufferEntry);
}

void MyPipeline::initStages()
{
    // Frustum culling stage
    {
        ascen::registry::DescriptorPoolEntry descriptorPoolEntry{};
        descriptorPoolEntry.mName = "DESCRIPTOR_POOL_COMPUTE";
        descriptorPoolEntry.mDescriptorTypeCounts = { { ascen::DESCRIPTOR_TYPE_SSBO, 3 } };

        mDescriptorPoolCompute = mEngine.registerResource<ascen::registry::DescriptorPoolEntry>(descriptorPoolEntry);

        ascen::registry::DescriptorLocation l0 { 0, ascen::DESCRIPTOR_TYPE_SSBO };
        ascen::registry::DescriptorLocation l1 { 1, ascen::DESCRIPTOR_TYPE_SSBO };
        ascen::registry::DescriptorLocation l2 { 2, ascen::DESCRIPTOR_TYPE_SSBO };

        ascen::registry::DescriptorBinding b0 { l0, ascen::SHADER_STAGE_COMPUTE };
        ascen::registry::DescriptorBinding b1 { l1, ascen::SHADER_STAGE_COMPUTE };
        ascen::registry::DescriptorBinding b2 { l2, ascen::SHADER_STAGE_COMPUTE };

        ascen::registry::DescriptorSetLayoutEntry descriptorSetLayoutEntry{};
        descriptorSetLayoutEntry.mName = "DESCRIPTOR_SET_LAYOUT_COMPUTE";
        descriptorSetLayoutEntry.mBindings = { b0, b1, b2 };

        mDescriptorSetLayoutCompute = mEngine.registerResource<ascen::registry::DescriptorSetLayoutEntry>(descriptorSetLayoutEntry);

        ascen::registry::DescriptorSetEntry descriptorSetEntry{};
        descriptorSetEntry.mName = "DESCRIPTOR_SET_COMPUTE";
        descriptorSetEntry.mPoolId = mDescriptorPoolCompute;
        descriptorSetEntry.mLayoutId = mDescriptorSetLayoutCompute;
        descriptorSetEntry.mResources =
        {
            { mBufferEntity,       l0, VK_WHOLE_SIZE },
            { mBufferVertexBounds, l1, VK_WHOLE_SIZE },
            { mBufferIndexBounds,  l2, VK_WHOLE_SIZE },
        };

        mDescriptorSetCompute = mEngine.registerResource<ascen::registry::DescriptorSetEntry>(descriptorSetEntry);

        ascen::registry::ComputePipelineEntry computePipelineEntry{};
        computePipelineEntry.mName = "PIPELINE_FRUSTUM_CULL";
        computePipelineEntry.mComputeShaderPath = "src/shaders/Simple/FrustumCullingShader.spv";
        computePipelineEntry.mDescriptorSetLayoutIds = { mDescriptorSetLayoutCompute };
        computePipelineEntry.mParams.mPushConstants =
        {
                { 0, { 0, sizeof(glm::mat4), ascen::SHADER_STAGE_COMPUTE, nullptr } }
        };

        mPipelineCompute = mEngine.registerResource<ascen::registry::ComputePipelineEntry>(computePipelineEntry);
    }

    // Render visible geometry stage
    {
        ascen::registry::DescriptorPoolEntry descriptorPoolEntry{};
        descriptorPoolEntry.mName = "DESCRIPTOR_POOL_GRAPHICS";
        descriptorPoolEntry.mDescriptorTypeCounts =
        {
            { ascen::DESCRIPTOR_TYPE_SSBO,    1 },
            { ascen::DESCRIPTOR_TYPE_SAMPLER, 1 },
            { ascen::DESCRIPTOR_TYPE_IMAGE,   1 },
        };

        mDescriptorPoolGraphics = mEngine.registerResource<ascen::registry::DescriptorPoolEntry>(descriptorPoolEntry);

        ascen::registry::DescriptorLocation l0 { 0, ascen::DESCRIPTOR_TYPE_SSBO };
        ascen::registry::DescriptorLocation l1 { 2, ascen::DESCRIPTOR_TYPE_SAMPLER };
        ascen::registry::DescriptorLocation l2 { 3, ascen::DESCRIPTOR_TYPE_IMAGE };

        ascen::registry::DescriptorBinding b0 { l0, ascen::SHADER_STAGE_VERTEX };
        ascen::registry::DescriptorBinding b1 { l1, ascen::SHADER_STAGE_PIXEL };
        ascen::registry::DescriptorBinding b2 { l2, ascen::SHADER_STAGE_PIXEL };

        ascen::registry::DescriptorSetLayoutEntry descriptorSetLayoutEntry{};
        descriptorSetLayoutEntry.mName = "DESCRIPTOR_SET_LAYOUT_GRAPHICS";
        descriptorSetLayoutEntry.mBindings = { b0, b1, b2 };

        mDescriptorSetLayoutGraphics = mEngine.registerResource<ascen::registry::DescriptorSetLayoutEntry>(descriptorSetLayoutEntry);

        ascen::registry::DescriptorSetEntry descriptorSetEntry{};
        descriptorSetEntry.mName = "DESCRIPTOR_SET_GRAPHICS";
        descriptorSetEntry.mPoolId = mDescriptorPoolGraphics;
        descriptorSetEntry.mLayoutId = mDescriptorSetLayoutGraphics;
        descriptorSetEntry.mResources =
        {
            { mBufferEntity, l0, VK_WHOLE_SIZE },
            { mSampler,      l1, 0 },
            { mTexture,      l2, 0 },
        };

        mDescriptorSetGraphics = mEngine.registerResource<ascen::registry::DescriptorSetEntry>(descriptorSetEntry);

        ascen::registry::GraphicsPipelineEntry graphicsPipelineEntryTriangles{};
        graphicsPipelineEntryTriangles.mName = "PIPELINE_RENDER_ENTITY";
        graphicsPipelineEntryTriangles.mVertexShaderPath = "src/shaders/Simple/VertexShader.spv";
        graphicsPipelineEntryTriangles.mPixelShaderPath = "src/shaders/Simple/PixelShader.spv";
        graphicsPipelineEntryTriangles.mVertexId = mVertexTriangles;
        graphicsPipelineEntryTriangles.mRenderPassId = mRenderPass;
        graphicsPipelineEntryTriangles.mDescriptorSetLayoutIds = { mDescriptorSetLayoutGraphics };
        graphicsPipelineEntryTriangles.mParams.mTopologyMode = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        graphicsPipelineEntryTriangles.mParams.mPolygonMode = VK_POLYGON_MODE_FILL;
        graphicsPipelineEntryTriangles.mParams.mCullMode = VK_CULL_MODE_BACK_BIT;
        graphicsPipelineEntryTriangles.mParams.mPushConstants =
        {
            { 0, { 0, sizeof(glm::mat4), ascen::SHADER_STAGE_VERTEX, nullptr } }
        };

        mPipelineGraphicsTriangles = mEngine.registerResource<ascen::registry::GraphicsPipelineEntry>(graphicsPipelineEntryTriangles);
    }

    // Render bounding boxes stage
    {
        ascen::registry::GraphicsPipelineEntry graphicsPipelineEntryBounds{};
        graphicsPipelineEntryBounds.mName = "PIPELINE_RENDER_BOUNDS";
        graphicsPipelineEntryBounds.mVertexShaderPath = "src/shaders/Simple/BoundsVertexShader.spv";
        graphicsPipelineEntryBounds.mPixelShaderPath = "src/shaders/Simple/BoundsPixelShader.spv";
        graphicsPipelineEntryBounds.mVertexId = mVertexPoint;
        graphicsPipelineEntryBounds.mRenderPassId = mRenderPass;
        graphicsPipelineEntryBounds.mDescriptorSetLayoutIds = { mDescriptorSetLayoutGraphics };
        graphicsPipelineEntryBounds.mParams.mTopologyMode = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        graphicsPipelineEntryBounds.mParams.mPolygonMode = VK_POLYGON_MODE_LINE;
        graphicsPipelineEntryBounds.mParams.mCullMode = VK_CULL_MODE_NONE;
        graphicsPipelineEntryBounds.mParams.mPushConstants =
        {
                { 0, { 0, sizeof(glm::mat4), ascen::SHADER_STAGE_VERTEX, nullptr } }
        };

        mPipelineGraphicsBoundingBoxes = mEngine.registerResource<ascen::registry::GraphicsPipelineEntry>(graphicsPipelineEntryBounds);
    }
}

void MyPipeline::initFramePasses()
{
    ascen::registry::FramePassEntry framePassComputeEntry;
    framePassComputeEntry.mName = "FRAMEPASS_COMPUTE";
    framePassComputeEntry.mType = ascen::FRAMEPASS_TYPE_COMPUTE;
    framePassComputeEntry.mDescriptorSetIds = { mDescriptorSetCompute };
    framePassComputeEntry.mPipelineId = mPipelineCompute;
    framePassComputeEntry.mComputeParams.mGroups[0] = (10'000 + 63) / 64;

    mEngine.registerResource<ascen::registry::FramePassEntry>(framePassComputeEntry);

    ascen::registry::FramePassEntry framePassGraphicsTrianglesEntry;
    framePassGraphicsTrianglesEntry.mName = "FRAMEPASS_GRAPHICS_TRIANGLES";
    framePassGraphicsTrianglesEntry.mType = ascen::FRAMEPASS_TYPE_GRAPHICS;
    framePassGraphicsTrianglesEntry.mDescriptorSetIds = { mDescriptorSetGraphics };
    framePassGraphicsTrianglesEntry.mPipelineId = mPipelineGraphicsTriangles;
    framePassGraphicsTrianglesEntry.mGraphicsParams.mDrawMode = ascen::FRAMEPASS_DRAW_MODE_TRIANGLES;
    framePassGraphicsTrianglesEntry.mGraphicsParams.mRenderPassId = mRenderPass;
    framePassGraphicsTrianglesEntry.mGraphicsParams.mRenderTargetId = mRenderTarget;
    framePassGraphicsTrianglesEntry.mGraphicsParams.mFrameBufferId = 0;
    framePassGraphicsTrianglesEntry.mGraphicsParams.mExtent = { 0, 0 };
    framePassGraphicsTrianglesEntry.mGraphicsParams.mVertexBufferIds = { mBufferVertex };
    framePassGraphicsTrianglesEntry.mGraphicsParams.mIndexBufferId = mBufferIndex;
    framePassGraphicsTrianglesEntry.mGraphicsParams.mIndirectBufferId = mBufferIndirect;

    mEngine.registerResource<ascen::registry::FramePassEntry>(framePassGraphicsTrianglesEntry);

    ascen::registry::FramePassEntry framePassGraphicsBoundingBoxesEntry;
    framePassGraphicsBoundingBoxesEntry.mName = "FRAMEPASS_GRAPHICS_BOUNDING_BOXES";
    framePassGraphicsBoundingBoxesEntry.mType = ascen::FRAMEPASS_TYPE_GRAPHICS;
    framePassGraphicsBoundingBoxesEntry.mDescriptorSetIds = { mDescriptorSetGraphics };
    framePassGraphicsBoundingBoxesEntry.mPipelineId = mPipelineGraphicsBoundingBoxes;
    framePassGraphicsBoundingBoxesEntry.mGraphicsParams.mDrawMode = ascen::FRAMEPASS_DRAW_MODE_LINES;
    framePassGraphicsBoundingBoxesEntry.mGraphicsParams.mRenderPassId = mRenderPass;
    framePassGraphicsBoundingBoxesEntry.mGraphicsParams.mRenderTargetId = mRenderTarget;
    framePassGraphicsBoundingBoxesEntry.mGraphicsParams.mFrameBufferId = 0;
    framePassGraphicsBoundingBoxesEntry.mGraphicsParams.mExtent = { 0, 0 };
    framePassGraphicsBoundingBoxesEntry.mGraphicsParams.mVertexBufferIds = { mBufferVertexBounds };
    framePassGraphicsBoundingBoxesEntry.mGraphicsParams.mIndexBufferId = mBufferIndexBounds;
    framePassGraphicsBoundingBoxesEntry.mGraphicsParams.mIndirectBufferId = mBufferIndirect;

    mEngine.registerResource<ascen::registry::FramePassEntry>(framePassGraphicsBoundingBoxesEntry);
}
