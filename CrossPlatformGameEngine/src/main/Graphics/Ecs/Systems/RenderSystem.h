#pragma once

#include "../../../EcsSystem/SystemManager/System/System_I.h"

#include "../Components/ModelComponent.h"
#include "../Components/TransformComponent.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vulkan/vulkan.h>

struct alignas(16) GPUInstance
{
	glm::mat4 mTransform = glm::mat4(1.0);
};

struct ModelData
{
	uint32_t mModelId = 0;
	uint32_t mMeshCount = 0;

	std::vector<uint32_t> mVertexOffsets;
	std::vector<uint32_t> mIndexCounts;
	std::vector<uint32_t> mIndexOffsets;
	std::vector<uint32_t> mTransformOffsets;
};

struct MeshKey {
	uint32_t modelId;
	uint32_t meshIndex;

	bool operator==(const MeshKey& other) const noexcept {
		return modelId == other.modelId && meshIndex == other.meshIndex;
	}
};

// For unordered_map:
struct MeshKeyHasher {
	size_t operator()(const MeshKey& k) const noexcept {
		return (static_cast<size_t>(k.modelId) << 32) ^ k.meshIndex;
	}
};

class RenderSystem : public System_I
{
public:
	RenderSystem(std::unordered_map<uint32_t, ModelData>* modelData) :
		mModelData(modelData) {}

	void update(float deltaTime) override
	{
		mMeshGroups.clear();
		mInstances.clear();
		mDrawCommands.clear();

		for (EntityId e : mEntities)
		{
			auto& model = mComponentManager->getComponent<ModelComponent>(e);
			if (model.mIsHidden) continue;

			ModelData& modelData = mModelData->at(model.mModelId);
			auto& transform = mComponentManager->getComponent<TransformComponent>(e);

			glm::mat4 T0 = glm::translate(glm::mat4(1.0f), transform.mPosition);
			glm::mat4 R0 = glm::toMat4(glm::quat(transform.mRotation));
			glm::mat4 S0 = glm::scale(glm::mat4(1.0f), transform.mScale);

			glm::mat4 modelMatrix = T0 * R0 * S0;

			for (uint32_t i = 0; i < modelData.mMeshCount; i++)
			{
				GPUInstance instance{};

				const auto& meshTransform = model.mMeshTransforms[i];

				glm::mat4 T1 = glm::translate(glm::mat4(1.0f), meshTransform.mPosition);
				glm::mat4 R1 = glm::toMat4(glm::quat(meshTransform.mRotation));
				glm::mat4 S1 = glm::scale(glm::mat4(1.0f), meshTransform.mScale);

				glm::mat4 meshMatrix = T1 * R1 * S1;

				instance.mTransform = modelMatrix * meshMatrix;

				MeshKey key{ model.mModelId, i };
				mMeshGroups[key].emplace_back(std::move(instance));
			}
		}


		uint32_t firstInstance = 0;
		for (auto& [key, instances] : mMeshGroups)
		{
			const ModelData& modelData = mModelData->at(key.modelId);
			uint32_t meshIndex = key.meshIndex;

			VkDrawIndexedIndirectCommand cmd{};
			cmd.vertexOffset = static_cast<int32_t>(modelData.mVertexOffsets[meshIndex]);
			cmd.firstIndex = modelData.mIndexOffsets[meshIndex];
			cmd.indexCount = modelData.mIndexCounts[meshIndex];
			cmd.firstInstance = firstInstance;
			cmd.instanceCount = static_cast<uint32_t>(instances.size());

			mDrawCommands.emplace_back(cmd);
			mInstances.insert(mInstances.end(), instances.begin(), instances.end());

			firstInstance += static_cast<uint32_t>(instances.size());
		}

	}

	const std::vector<GPUInstance>& getInstances() const
	{
		return mInstances;
	}

	const std::vector<VkDrawIndexedIndirectCommand>& getDrawCommands() const
	{
		return mDrawCommands;
	}

private:
	std::unordered_map<uint32_t, ModelData>* mModelData = nullptr;

	std::unordered_map<MeshKey, std::vector<GPUInstance>, MeshKeyHasher> mMeshGroups; // mesh ID -> instances
	std::vector<GPUInstance> mInstances;
	std::vector<VkDrawIndexedIndirectCommand> mDrawCommands;

};
