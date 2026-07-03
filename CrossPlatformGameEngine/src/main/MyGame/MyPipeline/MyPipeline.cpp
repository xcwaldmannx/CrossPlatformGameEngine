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

    mEngine.vertex().registerVertex({ "VERTEX_TRIANGLES", vertexBindingTri, vertexAttribTri });

    const ascen::VertexBinding vertexBindingPoint{ 0, sizeof(float) * 4, VK_VERTEX_INPUT_RATE_VERTEX };
    const std::vector<ascen::VertexAttribute> vertexAttribPoint =
    {
        { 0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0 },
    };

    mEngine.vertex().registerVertex({ "VERTEX_POINT", vertexBindingPoint, vertexAttribPoint });

    // Buffers
    // mEngine.resource().registerBuffer({ "BUFFER_CAMERA", ascen::BufferType::UNIFORM, 2, sizeof(glm::mat4) * 2 });

    mEngine.resource().registerBuffer({ "BUFFER_VERTEX", 100000, sizeof(float) * 8,
        ascen::BUFFER_USAGE_VERTEX | ascen::BUFFER_USAGE_TRANSFER_DST, ascen::BUFFER_MEMORY_LOCAL });
    mEngine.resource().registerBuffer({ "BUFFER_INDEX", 100000, sizeof(uint32_t),
        ascen::BUFFER_USAGE_INDEX | ascen::BUFFER_USAGE_TRANSFER_DST, ascen::BUFFER_MEMORY_LOCAL });

    mEngine.resource().registerBuffer({ "BUFFER_VERTEX_BOUNDS", 10'000, sizeof(float) * 4,
        ascen::BUFFER_USAGE_VERTEX | ascen::BUFFER_USAGE_STORAGE | ascen::BUFFER_USAGE_TRANSFER_SRC, ascen::BUFFER_MEMORY_LOCAL });
    mEngine.resource().registerBuffer({ "BUFFER_INDEX_BOUNDS", 10'000 * 24, sizeof(uint32_t),
    ascen::BUFFER_USAGE_INDEX | ascen::BUFFER_USAGE_STORAGE | ascen::BUFFER_USAGE_TRANSFER_SRC, ascen::BUFFER_MEMORY_LOCAL });

    mEngine.resource().registerBuffer({ "BUFFER_ENTITY", 1'000'000, sizeof(FrustumCullingSystem::Entity),
        ascen::BUFFER_USAGE_STORAGE | ascen::BUFFER_USAGE_TRANSFER_DST, ascen::BUFFER_MEMORY_LOCAL });
    mEngine.resource().registerBuffer({ "BUFFER_DRAWS", 1'000'000, sizeof(ascen::IndexedIndirectDraw),
        ascen::BUFFER_USAGE_INDIRECT | ascen::BUFFER_USAGE_STORAGE | ascen::BUFFER_USAGE_TRANSFER_DST, ascen::BUFFER_MEMORY_LOCAL });

    // Samplers
    mEngine.resource().registerSampler({ "SAMPLER" });

    // Textures
    mEngine.resource().registerTexture({ "TEXTURE", ascen::TextureType::IMAGE, 1024, 1024, 1 });

    // Render Targets

    ascen::RenderPass::Attachment colorAttachment{};
    colorAttachment.mType = ascen::ATTACHMENT_PRESENT;
    colorAttachment.mFormat = ascen::FORMAT_RGBA8_SRGB;
    colorAttachment.mLoadOp = ascen::LOAD_OP_CLEAR;
    colorAttachment.mStoreOp = ascen::STORE_OP_NA;

    ascen::RenderPass::Attachment depthAttachment{};
    depthAttachment.mType = ascen::ATTACHMENT_DEPTH;
    depthAttachment.mFormat = mEngine.getDepthFormat();
    depthAttachment.mLoadOp = ascen::LOAD_OP_CLEAR;

    ascen::RenderPass::SubPass subPass{};
    subPass.mBindPoint = ascen::BIND_POINT_GRAPHICS;
    subPass.mColorAttachmentIndices = { 0 };
    subPass.mDepthAttachmentIndex = 1;

    ascen::RenderPass::SubPassDependency subPassDependency{};
    subPassDependency.mSrcSubpass = 0;
    subPassDependency.mDstSubpass = 0;
    subPassDependency.mSrcAccessMask = ascen::ACCESS_NONE;
    subPassDependency.mDstAccessMask = ascen::ACCESS_COLOR_ATTACHMENT_WRITE | ascen::ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE;
    subPassDependency.mSrcStageMask = ascen::PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT | ascen::PIPELINE_STAGE_EARLY_FRAGMENT_TESTS;
    subPassDependency.mDstStageMask = ascen::PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT | ascen::PIPELINE_STAGE_EARLY_FRAGMENT_TESTS;

    ascen::RenderPassEntry renderPassEntry{};
    renderPassEntry.mName = "RENDER_PASS";
    renderPassEntry.mAttachments = { colorAttachment, depthAttachment };
    renderPassEntry.mSubPasses = { subPass };
    renderPassEntry.mSubPassDependencies = { subPassDependency };

    mEngine.render().registerRenderPass(renderPassEntry);

    ascen::RenderTargetEntry renderTargetEntry{};
    renderTargetEntry.mName = "RENDER_TARGET";
    renderTargetEntry.mFormat = mEngine.getImageFormat();
    renderTargetEntry.mImages = mEngine.getPresentImages();
    renderTargetEntry.mImageCount = mEngine.getPresentImages().size();

    mEngine.render().registerRenderTarget(renderTargetEntry);
}

void MyPipeline::initStages()
{
    // Frustum culling stage
    {
        mEngine.descriptor().registerDescriptor({ "BUFFER_ENTITY", "DESC_FRUSTUM_CULL",
            0x00, VK_WHOLE_SIZE, ascen::DescriptorType::SSBO, ascen::DescriptorStage::COMPUTE });

        mEngine.descriptor().registerDescriptor({ "BUFFER_VERTEX_BOUNDS", "DESC_FRUSTUM_CULL",
            0x01, VK_WHOLE_SIZE, ascen::DescriptorType::SSBO, ascen::DescriptorStage::COMPUTE });

        mEngine.descriptor().registerDescriptor({ "BUFFER_INDEX_BOUNDS", "DESC_FRUSTUM_CULL",
            0x02, VK_WHOLE_SIZE, ascen::DescriptorType::SSBO, ascen::DescriptorStage::COMPUTE });

        ascen::ComputePipelineEntry computePipelineEntry;
        computePipelineEntry.mName = "PIPELINE_FRUSTUM_CULL";
        computePipelineEntry.mComputeShader = "src/shaders/Simple/FrustumCullingShader.spv";
        computePipelineEntry.mDescriptorSetLayouts = { "DESC_FRUSTUM_CULL" };
        computePipelineEntry.mParams.mPushConstantRange =
        {
            "PUSH_0", 0, sizeof(glm::mat4)
        };

        mEngine.pipeline().registerComputePipeline(computePipelineEntry);
    }

    // Render visible geometry stage
    {
        mEngine.descriptor().registerDescriptor({ "BUFFER_ENTITY", "DESC_RENDER_ENTITY",
            0x00, VK_WHOLE_SIZE, ascen::DescriptorType::SSBO, ascen::DescriptorStage::VERTEX });

        mEngine.descriptor().registerDescriptor({ "SAMPLER", "DESC_RENDER_ENTITY",
            0x02, 0,ascen::DescriptorType::SAMPLER, ascen::DescriptorStage::PIXEL });

        mEngine.descriptor().registerDescriptor({ "TEXTURE", "DESC_RENDER_ENTITY",
            0x03, 0,ascen::DescriptorType::IMAGE, ascen::DescriptorStage::PIXEL });

        ascen::GraphicsPipelineEntry graphicsPipelineEntryTriangles;
        graphicsPipelineEntryTriangles.mName = "PIPELINE_RENDER_ENTITY";
        graphicsPipelineEntryTriangles.mVertexShader = "src/shaders/Simple/VertexShader.spv";
        graphicsPipelineEntryTriangles.mPixelShader = "src/shaders/Simple/PixelShader.spv";
        graphicsPipelineEntryTriangles.mVertex = "VERTEX_TRIANGLES";
        graphicsPipelineEntryTriangles.mDescriptorSetLayouts = { "DESC_RENDER_ENTITY" };
        graphicsPipelineEntryTriangles.mParams.mTopologyMode = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        graphicsPipelineEntryTriangles.mParams.mPolygonMode = VK_POLYGON_MODE_FILL;
        graphicsPipelineEntryTriangles.mParams.mCullMode = VK_CULL_MODE_BACK_BIT;
        graphicsPipelineEntryTriangles.mParams.mPushConstantRange =
        {
            "PUSH_0", 0, sizeof(glm::mat4)
        };

        mEngine.pipeline().registerGraphicsPipeline(graphicsPipelineEntryTriangles);
    }

    // Render bounding boxes stage
    {
        ascen::GraphicsPipelineEntry graphicsPipelineEntryBounds;
        graphicsPipelineEntryBounds.mName = "PIPELINE_RENDER_BOUNDS";
        graphicsPipelineEntryBounds.mVertexShader = "src/shaders/Simple/BoundsVertexShader.spv";
        graphicsPipelineEntryBounds.mPixelShader = "src/shaders/Simple/BoundsPixelShader.spv";
        graphicsPipelineEntryBounds.mVertex = "VERTEX_POINT";
        graphicsPipelineEntryBounds.mParams.mTopologyMode = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        graphicsPipelineEntryBounds.mParams.mPolygonMode = VK_POLYGON_MODE_LINE;
        graphicsPipelineEntryBounds.mParams.mCullMode = VK_CULL_MODE_NONE;
        graphicsPipelineEntryBounds.mParams.mPushConstantRange =
        {
            "PUSH_0", 0, sizeof(glm::mat4)
        };

        mEngine.pipeline().registerGraphicsPipeline(graphicsPipelineEntryBounds);
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
