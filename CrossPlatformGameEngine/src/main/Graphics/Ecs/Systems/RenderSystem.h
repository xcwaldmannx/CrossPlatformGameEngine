#pragma once

#include "../../../EcsSystem/SystemManager/System/System_I.h"

#include "../Components/ModelComponent.h"
#include "../Components/TransformComponent.h"

#include "../../Resource/Buffer/Indirect/IndirectBuffer.h"

#include <cstdint>

#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <vulkan/vulkan.h>

namespace ascen
{

	struct alignas(16) GPUEntity
	{
		glm::vec3 mPosition;
		uint32_t _pad0;

		glm::vec3 mRotation;
		uint32_t _pad1;

		glm::vec3 mScale;
		uint32_t mMeshCount;

		uint32_t mMeshOffset;
		uint32_t mInstanceLocalOffset;
		uint32_t _pad2[2];
	};

	struct alignas(16) GPUMesh
	{
		glm::vec3 mPosition;
		uint32_t _pad0;

		glm::vec3 mRotation;
		uint32_t _pad1;

		glm::vec3 mScale;
		uint32_t mTransformOffset;

		uint32_t mTextureOffset;
		uint32_t mInstanceBaseOffset;
		uint32_t _pad2[2];

		glm::vec3 mBoundsPos;
		uint32_t _pad3;

		glm::vec3 mBoundsNeg;
		uint32_t _pad4;
	};

	struct alignas(16) GPUInstance
	{
		glm::mat4 mTransform;
		uint32_t mTextureIndex;
		bool mIsVisible;
		uint32_t _pad[2];
	};

	struct ModelData
	{
		uint32_t mModelId = 0;
		uint32_t mMeshCount = 0;

		std::vector<uint32_t> mVertexOffsets;
		std::vector<uint32_t> mIndexCounts;
		std::vector<uint32_t> mIndexOffsets;
		std::vector<uint32_t> mTransformOffsets;

		std::vector<glm::vec3> mBoundsPos;
		std::vector<glm::vec3> mBoundsNeg;
	};

	struct MeshKey
	{
		uint32_t modelId;
		uint32_t meshIndex;

		bool operator==(const MeshKey& other) const noexcept
		{
			return modelId == other.modelId && meshIndex == other.meshIndex;
		}
	};

	struct MeshKeyHasher
	{
		size_t operator()(const MeshKey& k) const noexcept
		{
			return (static_cast<size_t>(k.modelId) << 32) ^ k.meshIndex;
		}
	};

	class RenderSystem : public System_I
	{
	public:
		RenderSystem(std::unordered_map<uint32_t, ModelData>* modelData);

		void update(float deltaTime) override;

		void updateModels(std::unordered_map<uint32_t, ModelData>& modelData);

		const std::vector<GPUEntity>& getEntities() const;
		const std::vector<GPUMesh>& getMeshes() const;
		const std::vector<IndirectBuffer::IndexedIndirectCommand> getMeshDraws() const;
		const std::vector<IndirectBuffer::IndirectCommand> getBBoxDraws() const;

	private:
		std::unordered_map<uint32_t, ModelData>* mModelData = nullptr;

		// std::unordered_map<MeshKey, std::vector<GPUInstance>, MeshKeyHasher> mMeshGroups; // mesh ID -> instances
		std::vector<GPUEntity> mGPUEntities;
		std::vector<GPUMesh> mGPUMeshes;
		std::vector<IndirectBuffer::IndexedIndirectCommand> mMeshDraws;
		std::vector<IndirectBuffer::IndirectCommand> mBBoxDraws;

	};

}
