#pragma once

#include "../../Utility/ImageLoader/ImageLoader.h"
#include "../DefaultGraphicsPipeline.h"
#include "../../Graphics/Vertex/TextureVertex.h"

#include "../../Graphics/Ecs/Systems/RenderSystem.h"

#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct GPUCamera
{
	glm::mat4 mView;
	glm::mat4 mProj;
};

// per-instance data for an entity, stored in gpu
struct alignas(16) TestGPUInstanceData
{
	glm::mat4 mTransform = glm::mat4(1.0);
};

class TestGraphicsPipeline : public ascen::DefaultGraphicsPipeline<ascen::TextureVertex>
{
public:
	TestGraphicsPipeline(
		WindowManager* windowManager,
		const std::string vertexShaderFilepath,
		const std::string pixelShaderFilepath,
		const std::vector<float> vertices,
		const std::vector<uint32_t> indices,
		const std::vector<float> transforms);

	void createShaderResources() override;
	void destroyShaderResources() override;
	void createDescriptorResources() override;
	void destroyDescriptorResources() override;

	void record(uint32_t currentImage) override;

	void updateCamera(glm::mat4& transform);
	void updateInstances(const std::vector<GPUInstance>& instances);
	void updateDrawCommands(const std::vector<VkDrawIndexedIndirectCommand>& drawCommands);

private:
	void createBuffers();
	void createTextures();
	void createSamplers();

private:
	// ubo
	std::shared_ptr<ascen::Buffer> mCameraBuffer = nullptr;

	// ssbo static
	std::shared_ptr<ascen::Buffer> mVertexBuffer = nullptr;
	std::shared_ptr<ascen::Buffer> mIndexBuffer = nullptr;
	std::shared_ptr<ascen::Buffer> mTransformBuffer = nullptr;

	// ssbo dynamic
	std::shared_ptr<ascen::Buffer> mInstanceBuffer = nullptr;

	std::shared_ptr<ascen::Buffer> mIndirectBuffer = nullptr;

	std::shared_ptr<ascen::Texture> mDepthTexture = nullptr;

	std::shared_ptr<ascen::Texture> mTexture = nullptr;
	std::shared_ptr<ascen::Sampler> mSampler = nullptr;

	const std::unordered_map<uint32_t, uint32_t> mModelInfo;
	const std::vector<float> mVertices;
	const std::vector<uint32_t> mIndices;
	const std::vector<float> mTransforms;
};