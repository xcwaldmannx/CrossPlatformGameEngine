#include "MyPipeline.h"

#include "../Ecs/Systems/FrustumCullingSystem.h"

#include <glm/glm.hpp>

#include "../../Engine/Device/Physical/PhysicalDevice.h"

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

    mEngine.registerResource<ascen::registry::BufferEntry>({ "BUFFER_VERTEX", 100000, sizeof(float) * 8,
        ascen::BUFFER_USAGE_VERTEX | ascen::BUFFER_USAGE_TRANSFER_DST, ascen::BUFFER_MEMORY_LOCAL });
    mEngine.registerResource<ascen::registry::BufferEntry>({ "BUFFER_INDEX", 100000, sizeof(uint32_t),
        ascen::BUFFER_USAGE_INDEX | ascen::BUFFER_USAGE_TRANSFER_DST, ascen::BUFFER_MEMORY_LOCAL });

    mBufferVertexBounds = mEngine.registerResource<ascen::registry::BufferEntry>({ "BUFFER_VERTEX_BOUNDS", 10'000, sizeof(float) * 4,
        ascen::BUFFER_USAGE_VERTEX | ascen::BUFFER_USAGE_STORAGE | ascen::BUFFER_USAGE_TRANSFER_SRC, ascen::BUFFER_MEMORY_LOCAL });
    mBufferIndexBounds = mEngine.registerResource<ascen::registry::BufferEntry>({ "BUFFER_INDEX_BOUNDS", 10'000 * 24, sizeof(uint32_t),
    ascen::BUFFER_USAGE_INDEX | ascen::BUFFER_USAGE_STORAGE | ascen::BUFFER_USAGE_TRANSFER_SRC, ascen::BUFFER_MEMORY_LOCAL });

    mBufferEntity = mEngine.registerResource<ascen::registry::BufferEntry>({ "BUFFER_ENTITY", 1'000'000, sizeof(FrustumCullingSystem::Entity),
        ascen::BUFFER_USAGE_STORAGE | ascen::BUFFER_USAGE_TRANSFER_DST, ascen::BUFFER_MEMORY_LOCAL });
    mEngine.registerResource<ascen::registry::BufferEntry>({ "BUFFER_DRAWS", 1'000'000, sizeof(ascen::IndexedIndirectDraw),
        ascen::BUFFER_USAGE_INDIRECT | ascen::BUFFER_USAGE_STORAGE | ascen::BUFFER_USAGE_TRANSFER_DST, ascen::BUFFER_MEMORY_LOCAL });

    // Samplers
    mSampler = mEngine.registerResource<ascen::registry::SamplerEntry>({ "SAMPLER" });

    // Textures
    mTexture = mEngine.registerResource<ascen::registry::TextureEntry>({ "TEXTURE", ascen::TextureType::IMAGE, 1024, 1024, 1 });

    // Render Targets

    ascen::renderpass::Attachment colorAttachment{};
    colorAttachment.mType = ascen::ATTACHMENT_PRESENT;
    colorAttachment.mFormat = ascen::FORMAT_RGBA8_SRGB;
    colorAttachment.mLoadOp = ascen::LOAD_OP_CLEAR;
    colorAttachment.mStoreOp = ascen::STORE_OP_NA;

    ascen::renderpass::Attachment depthAttachment{};
    depthAttachment.mType = ascen::ATTACHMENT_DEPTH;
    depthAttachment.mFormat = mEngine.getDepthFormat();
    depthAttachment.mLoadOp = ascen::LOAD_OP_CLEAR;

    ascen::renderpass::SubPass subPass{};
    subPass.mBindPoint = ascen::BIND_POINT_GRAPHICS;
    subPass.mColorAttachmentIndices = { 0 };
    subPass.mDepthAttachmentIndex = 1;

    ascen::renderpass::SubPassDependency subPassDependency{};
    subPassDependency.mSrcSubpass = 0;
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

    mEngine.registerResource<ascen::registry::RenderPassEntry>(renderPassEntry);

    ascen::registry::RenderTargetEntry renderTargetEntry{};
    renderTargetEntry.mName = "RENDER_TARGET";
    renderTargetEntry.mFormat = mEngine.getImageFormat();
    renderTargetEntry.mImages = mEngine.getPresentImages();
    renderTargetEntry.mImageCount = mEngine.getPresentImages().size();

    mEngine.registerResource<ascen::registry::RenderTargetEntry>(renderTargetEntry);
}

void MyPipeline::initStages()
{
    // Frustum culling stage
    {
        ascen::registry::DescriptorPoolEntry descriptorPoolEntry{};
        descriptorPoolEntry.mName = "DESCRIPTOR_POOL_COMPUTE";
        descriptorPoolEntry.mDescriptorTypeCounts = { { ascen::DescriptorType::SSBO, 3 } };

        mDescriptorPoolCompute = mEngine.registerResource<ascen::registry::DescriptorPoolEntry>(descriptorPoolEntry);

        ascen::registry::DescriptorLocation l0 { 0, ascen::DescriptorType::SSBO };
        ascen::registry::DescriptorLocation l1 { 1, ascen::DescriptorType::SSBO };
        ascen::registry::DescriptorLocation l2 { 2, ascen::DescriptorType::SSBO };

        ascen::registry::DescriptorBinding b0 { l0, ascen::DescriptorStage::COMPUTE };
        ascen::registry::DescriptorBinding b1 { l1, ascen::DescriptorStage::COMPUTE };
        ascen::registry::DescriptorBinding b2 { l2, ascen::DescriptorStage::COMPUTE };

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

        ascen::registry::ComputePipelineEntry computePipelineEntry;
        computePipelineEntry.mName = "PIPELINE_FRUSTUM_CULL";
        computePipelineEntry.mComputeShaderPath = "src/shaders/Simple/FrustumCullingShader.spv";
        computePipelineEntry.mDescriptorSetLayoutIds = { mDescriptorSetLayoutCompute };
        computePipelineEntry.mParams.mPushConstantRange =
        {
            0, sizeof(glm::mat4)
        };

        mEngine.registerResource<ascen::registry::ComputePipelineEntry>(computePipelineEntry);
    }

    // Render visible geometry stage
    {
        ascen::registry::DescriptorPoolEntry descriptorPoolEntry{};
        descriptorPoolEntry.mName = "DESCRIPTOR_POOL_GRAPHICS";
        descriptorPoolEntry.mDescriptorTypeCounts =
        {
            { ascen::DescriptorType::SSBO,    1 },
            { ascen::DescriptorType::SAMPLER, 1 },
            { ascen::DescriptorType::IMAGE,   1 },
        };

        mDescriptorPoolGraphics = mEngine.registerResource<ascen::registry::DescriptorPoolEntry>(descriptorPoolEntry);

        ascen::registry::DescriptorLocation l0 { 0, ascen::DescriptorType::SSBO };
        ascen::registry::DescriptorLocation l1 { 2, ascen::DescriptorType::SAMPLER };
        ascen::registry::DescriptorLocation l2 { 3, ascen::DescriptorType::IMAGE };

        ascen::registry::DescriptorBinding b0 { l0, ascen::DescriptorStage::VERTEX };
        ascen::registry::DescriptorBinding b1 { l1, ascen::DescriptorStage::PIXEL };
        ascen::registry::DescriptorBinding b2 { l2, ascen::DescriptorStage::PIXEL };

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

        mDescriptorSetCompute = mEngine.registerResource<ascen::registry::DescriptorSetEntry>(descriptorSetEntry);

        ascen::registry::GraphicsPipelineEntry graphicsPipelineEntryTriangles;
        graphicsPipelineEntryTriangles.mName = "PIPELINE_RENDER_ENTITY";
        graphicsPipelineEntryTriangles.mVertexShaderPath = "src/shaders/Simple/VertexShader.spv";
        graphicsPipelineEntryTriangles.mPixelShaderPath = "src/shaders/Simple/PixelShader.spv";
        graphicsPipelineEntryTriangles.mVertexId = mVertexTriangles;
        graphicsPipelineEntryTriangles.mDescriptorSetLayoutIds = { mDescriptorSetLayoutGraphics };
        graphicsPipelineEntryTriangles.mParams.mTopologyMode = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        graphicsPipelineEntryTriangles.mParams.mPolygonMode = VK_POLYGON_MODE_FILL;
        graphicsPipelineEntryTriangles.mParams.mCullMode = VK_CULL_MODE_BACK_BIT;
        graphicsPipelineEntryTriangles.mParams.mPushConstantRange =
        {
            0, sizeof(glm::mat4)
        };

        mEngine.registerResource<ascen::registry::GraphicsPipelineEntry>(graphicsPipelineEntryTriangles);
    }

    // Render bounding boxes stage
    {
        ascen::registry::GraphicsPipelineEntry graphicsPipelineEntryBounds;
        graphicsPipelineEntryBounds.mName = "PIPELINE_RENDER_BOUNDS";
        graphicsPipelineEntryBounds.mVertexShaderPath = "src/shaders/Simple/BoundsVertexShader.spv";
        graphicsPipelineEntryBounds.mPixelShaderPath = "src/shaders/Simple/BoundsPixelShader.spv";
        graphicsPipelineEntryBounds.mVertexId = mVertexPoint;
        graphicsPipelineEntryBounds.mParams.mTopologyMode = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        graphicsPipelineEntryBounds.mParams.mPolygonMode = VK_POLYGON_MODE_LINE;
        graphicsPipelineEntryBounds.mParams.mCullMode = VK_CULL_MODE_NONE;
        graphicsPipelineEntryBounds.mParams.mPushConstantRange =
        {
            0, sizeof(glm::mat4)
        };

        mEngine.registerResource<ascen::registry::GraphicsPipelineEntry>(graphicsPipelineEntryBounds);
    }
}

void MyPipeline::initFramePasses()
{
    mEngine.frame().registerCompute(
        { "FRAMEPASS_FRUSTUM_CULL",
        { "DESC_FRUSTUM_CULL" },
        "PIPELINE_FRUSTUM_CULL",
        {
            { "BUFFER_ENTITY",        ascen::ResourceUsage::BUFFER_STORAGE, ascen::ResourceAccess::WRITE, ascen::ResourceStage::COMPUTE },
            { "BUFFER_VERTEX_BOUNDS", ascen::ResourceUsage::BUFFER_STORAGE, ascen::ResourceAccess::WRITE, ascen::ResourceStage::COMPUTE },
            { "BUFFER_INDEX_BOUNDS",  ascen::ResourceUsage::BUFFER_STORAGE, ascen::ResourceAccess::WRITE, ascen::ResourceStage::COMPUTE },
        },
        { (1'000'000 + 63) / 64, 1, 1 }});


    mEngine.frame().registerGraphics(
        { "FRAMEPASS_RENDER_ENTITY",
        { "DESC_RENDER_ENTITY" },
        "PIPELINE_RENDER_ENTITY",
        {
            { "BUFFER_VERTEX", ascen::ResourceUsage::BUFFER_VERTEX,   ascen::ResourceAccess::READ, ascen::ResourceStage::VERTEX   },
            { "BUFFER_INDEX",  ascen::ResourceUsage::BUFFER_INDEX,    ascen::ResourceAccess::READ, ascen::ResourceStage::VERTEX   },
            { "BUFFER_DRAWS",  ascen::ResourceUsage::BUFFER_INDIRECT, ascen::ResourceAccess::READ, ascen::ResourceStage::VERTEX   },
            { "BUFFER_ENTITY", ascen::ResourceUsage::BUFFER_STORAGE,  ascen::ResourceAccess::READ, ascen::ResourceStage::VERTEX   },
            { "TEXTURE",       ascen::ResourceUsage::IMAGE_SAMPLED,   ascen::ResourceAccess::READ, ascen::ResourceStage::FRAGMENT },
        },
        ascen::GraphicsMode::MESH });

    mEngine.frame().registerGraphics(
        { "FRAMEPASS_RENDER_BOUNDS",
        {},
        "PIPELINE_RENDER_BOUNDS",
        {
            { "BUFFER_VERTEX_BOUNDS", ascen::ResourceUsage::BUFFER_VERTEX, ascen::ResourceAccess::READ, ascen::ResourceStage::VERTEX },
            { "BUFFER_INDEX_BOUNDS",  ascen::ResourceUsage::BUFFER_INDEX,  ascen::ResourceAccess::READ, ascen::ResourceStage::VERTEX },
        },
        ascen::GraphicsMode::LINES });
}
