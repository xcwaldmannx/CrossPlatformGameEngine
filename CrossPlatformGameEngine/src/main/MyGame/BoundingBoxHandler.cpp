#include "BoundingBoxHandler.h"

BoundingBoxHandler::BoundingBoxHandler(ascen::Engine& engine) :
    mEngine(engine)
{
    ascen::BufferEntry inputBufferEntry{};
    inputBufferEntry.mName = "BBOX_BUFFER_INPUT";
    inputBufferEntry.mType = ascen::BufferType::STORAGE;
    inputBufferEntry.mCapacity = 4;
    inputBufferEntry.mStride = sizeof(BBoxInput);
    mEngine.resource().registerBuffer(inputBufferEntry);

    ascen::BufferEntry outputBufferEntry{};
    outputBufferEntry.mName = "BBOX_BUFFER_OUTPUT";
    outputBufferEntry.mType = ascen::BufferType::STORAGE;
    outputBufferEntry.mCapacity = 4;
    outputBufferEntry.mStride = sizeof(BBoxOutput);
    mEngine.resource().registerBuffer(outputBufferEntry);

    ascen::DescriptorSetEntry cameraDescriptorSetEntry{};
    cameraDescriptorSetEntry.mSetName = "BBOX_DESC_COMPUTE";
    cameraDescriptorSetEntry.mResourceName = "ENGINE_BUFFER_CAMERA";
    cameraDescriptorSetEntry.mSlot = 0x00;
    cameraDescriptorSetEntry.mSize = sizeof(glm::mat4) * 2;
    cameraDescriptorSetEntry.mType = ascen::DescriptorType::UBO_DYNAMIC;
    cameraDescriptorSetEntry.mStage = ascen::DescriptorStage::COMPUTE;
    mEngine.descriptor().registerDescriptor(cameraDescriptorSetEntry);

    ascen::DescriptorSetEntry inputDescriptorSetEntry{};
    inputDescriptorSetEntry.mSetName = "BBOX_DESC_COMPUTE";
    inputDescriptorSetEntry.mResourceName = "BBOX_BUFFER_INPUT";
    inputDescriptorSetEntry.mSlot = 0x01;
    inputDescriptorSetEntry.mSize = VK_WHOLE_SIZE;
    inputDescriptorSetEntry.mType = ascen::DescriptorType::SSBO;
    inputDescriptorSetEntry.mStage = ascen::DescriptorStage::COMPUTE;
    mEngine.descriptor().registerDescriptor(inputDescriptorSetEntry);

    ascen::DescriptorSetEntry outputDescriptorSetEntry{};
    outputDescriptorSetEntry.mSetName = "BBOX_DESC_COMPUTE";
    outputDescriptorSetEntry.mResourceName = "BBOX_BUFFER_OUTPUT";
    outputDescriptorSetEntry.mSlot = 0x02;
    outputDescriptorSetEntry.mSize = VK_WHOLE_SIZE;
    outputDescriptorSetEntry.mType = ascen::DescriptorType::SSBO;
    outputDescriptorSetEntry.mStage = ascen::DescriptorStage::COMPUTE;
    mEngine.descriptor().registerDescriptor(outputDescriptorSetEntry);

    ascen::ComputePipelineEntry computePipelineEntry{};
    computePipelineEntry.mName = "BBOX_PIPELINE_COMPUTE";
    computePipelineEntry.mComputeShader = "src/shaders/BoundingBoxCS.spv";
    computePipelineEntry.mDescriptorSetLayouts = { "BBOX_DESC_COMPUTE" };
    mEngine.pipeline().registerComputePipeline(computePipelineEntry);

    ascen::ComputeFramePassEntry framePassEntry{};
    framePassEntry.mName = "BBOX_FRAMEPASS_COMPUTE";
    framePassEntry.mPipeline = "BBOX_PIPELINE_COMPUTE";
    framePassEntry.mDescriptorSets = { "BBOX_DESC_COMPUTE" };
    framePassEntry.mResources =
    {
        { "ENGINE_BUFFER_CAMERA", ascen::ResourceUsage::BUFFER_UNIFORM, ascen::ResourceAccess::READ },
        { "BBOX_BUFFER_INPUT",    ascen::ResourceUsage::BUFFER_STORAGE, ascen::ResourceAccess::READ },
        { "BBOX_BUFFER_OUTPUT",   ascen::ResourceUsage::BUFFER_STORAGE, ascen::ResourceAccess::WRITE }
    };
    framePassEntry.mGroups = { (1000 + 63) / 64, 1, 1 };
    mEngine.frame().registerCompute(framePassEntry);
}

void BoundingBoxHandler::update()
{
    mInputs =
    {
        {0, 1}, {1, 2}, {2, 3}, {3, 4}
    };

    mEngine.resource().updateBuffer("BBOX_BUFFER_INPUT", &mInputs, mInputs.size(), sizeof(BBoxInput));
}

void BoundingBoxHandler::print()
{
    auto outputs = mEngine.resource().getMappedBuffer<BBoxOutput>("BBOX_BUFFER_OUTPUT");

    for (const auto& output : outputs)
    {
        std::cout << "output: " << output.sum << ", ";
    }

    if (!mOutputs.empty()) std::cout << std::endl;
}
