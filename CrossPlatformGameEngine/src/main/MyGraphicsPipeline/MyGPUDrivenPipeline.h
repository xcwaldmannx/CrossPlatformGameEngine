#pragma once

// ubo for camera
struct alignas(16) UBOStruct
{
	glm::mat4 mView;
	glm::mat4 mProj;
};

// model data
struct GPUModelData
{
	uint32_t mVertexOffset = 0;
	uint32_t mVertexCount = 0;
	uint32_t mIndexOffset = 0;
	uint32_t mIndexCount = 0;
};

// per-instance data for an entity
struct GPUInstanceData
{
	glm::mat4 mTransform = glm::mat4(1.0);
	uint32_t mModelId = 0;
};

class MyGPUDrivenPipeline
{
	MyGPUDrivenPipeline();
};