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

class RenderSystem : public System_I
{
public:
	RenderSystem(std::unordered_map<uint32_t, ModelData>* modelData) :
		mModelData(modelData) {}

	void update(float deltaTime) override
	{
		mModelGroups.clear();
		mInstances.clear();
		mDrawCommands.clear();

		for (EntityId e : mEntities)
		{
			auto& model = mComponentManager->getComponent<ModelComponent>(e);

			if (model.mIsHidden) continue;

			auto& transform = mComponentManager->getComponent<TransformComponent>(e);

			glm::quat rotQuat = glm::quat(transform.mRotation);
			glm::mat4 rotMatrix = glm::toMat4(rotQuat);

			ModelData& modelData = mModelData->at(model.mModelId);

			for (uint32_t i = 0; i < modelData.mMeshCount; i++)
			{
				GPUInstance instance{};
				instance.mTransform =
					(glm::translate(glm::mat4(1.0f), transform.mPosition)
						* rotMatrix
						* glm::scale(glm::mat4(1.0f), transform.mScale));

				mModelGroups[model.mModelId].emplace_back(std::move(instance));
			}

		}

		uint32_t firstInstance = 0;

		for (auto& [modelId, instances] : mModelGroups)
		{
			ModelData& modelData = mModelData->at(modelId);

			for (uint32_t i = 0; i < modelData.mMeshCount; i++)
			{
				VkDrawIndexedIndirectCommand cmd{};
				cmd.vertexOffset  = static_cast<int32_t>(modelData.mVertexOffsets[i]);
				cmd.firstIndex    = modelData.mIndexOffsets[i];
				cmd.indexCount    = modelData.mIndexCounts[i];
				cmd.firstInstance = firstInstance;
				cmd.instanceCount = static_cast<uint32_t>(instances.size());
				mDrawCommands.emplace_back(std::move(cmd));
			}

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

	std::unordered_map<uint32_t, std::vector<GPUInstance>> mModelGroups; // mesh ID -> instances
	std::vector<GPUInstance> mInstances;
	std::vector<VkDrawIndexedIndirectCommand> mDrawCommands;

};
