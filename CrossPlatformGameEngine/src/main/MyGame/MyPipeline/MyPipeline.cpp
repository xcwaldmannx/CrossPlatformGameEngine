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

    mEngine.vertex().registerVertex({ "VERTEX_TRIANGLES", vertexBindingTri, vertexAttribTri });

    // Buffers
    mEngine.resource().registerBuffer({ "BUFFER_CAMERA", ascen::BufferType::UNIFORM, 2, sizeof(glm::mat4) * 2 });

    mEngine.resource().registerBuffer({ "BUFFER_VERTEX", ascen::BufferType::VERTEX,  100000, sizeof(float) * 8 });
    mEngine.resource().registerBuffer({ "BUFFER_INDEX",  ascen::BufferType::INDEX,   100000, sizeof(uint32_t) });

    mEngine.resource().registerBuffer({ "BUFFER_ENTITY", ascen::BufferType::STORAGE,  1'000'000, sizeof(FrustumCullingSystem::Entity) });
    mEngine.resource().registerBuffer({ "BUFFER_DRAWS",  ascen::BufferType::INDIRECT, 1'000'000, sizeof(ascen::IndirectBuffer::IndexedIndirectCommand) });

    // Samplers
    mEngine.resource().registerSampler({ "SAMPLER" });

    // Textures
    mEngine.resource().registerTexture({ "TEXTURE", ascen::TextureType::IMAGE, 1024, 1024, 1 });
}

void MyPipeline::initStages()
{
    // Frustum culling stage
    // mEngine.descriptor().registerDescriptor({ "BUFFER_CAMERA", "DESC_FRUSTUM_CULL",
    //     0x00, sizeof(glm::mat4) * 2, ascen::DescriptorType::UBO_DYNAMIC, ascen::DescriptorStage::COMPUTE });

    mEngine.descriptor().registerDescriptor({ "BUFFER_ENTITY", "DESC_FRUSTUM_CULL",
        0x00, VK_WHOLE_SIZE, ascen::DescriptorType::SSBO, ascen::DescriptorStage::COMPUTE });

    ascen::ComputePipelineEntry computePipelineEntry;
    computePipelineEntry.mName = "PIPELINE_FRUSTUM_CULL";
    computePipelineEntry.mComputeShader = "src/shaders/Simple/FrustumCullingShader.spv";
    computePipelineEntry.mDescriptorSetLayouts = { "DESC_FRUSTUM_CULL" };
    computePipelineEntry.mParams.mPushConstantRange =
    {
        "PUSH_0", 0, sizeof(glm::mat4)
    };

    mEngine.pipeline().registerComputePipeline(computePipelineEntry);

    // Render visible geometry stage
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

void MyPipeline::initFramePasses()
{
    mEngine.frame().registerCompute(
        { "FRAMEPASS_FRUSTUM_CULL",
        { "DESC_FRUSTUM_CULL" },
        "PIPELINE_FRUSTUM_CULL",
        {
            { "BUFFER_CAMERA", ascen::ResourceUsage::BUFFER_UNIFORM, ascen::ResourceAccess::READ, ascen::ResourceStage::COMPUTE },
            { "BUFFER_ENTITY", ascen::ResourceUsage::BUFFER_STORAGE, ascen::ResourceAccess::WRITE, ascen::ResourceStage::COMPUTE },
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
            { "BUFFER_CAMERA", ascen::ResourceUsage::BUFFER_UNIFORM,  ascen::ResourceAccess::READ, ascen::ResourceStage::VERTEX   },
            { "BUFFER_ENTITY", ascen::ResourceUsage::BUFFER_STORAGE,  ascen::ResourceAccess::READ, ascen::ResourceStage::VERTEX   },
            { "TEXTURE",       ascen::ResourceUsage::IMAGE_SAMPLED,   ascen::ResourceAccess::READ, ascen::ResourceStage::FRAGMENT },
        },
        ascen::GraphicsMode::MESH });
}
