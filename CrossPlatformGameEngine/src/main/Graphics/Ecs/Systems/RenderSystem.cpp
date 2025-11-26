#include "RenderSystem.h"

#include <map>

using namespace ascen;

RenderSystem::RenderSystem(
	std::unordered_map<uint32_t, ModelData>* modelData) :
	mModelData(modelData)
{}

void RenderSystem::update(float deltaTime)
{
	mGPUEntities.clear();
	mGPUMeshes.clear();
	mDrawCommands.clear();

	std::map<uint32_t, std::vector<EntityId>> modelToEntities;

	for (EntityId e : mEntities)
	{
		const auto& model = mComponentManager->getComponent<ModelComponent>(e);
		if (!model.mIsHidden) modelToEntities[model.mModelId].push_back(e);
	}

	uint32_t firstInstance = 0;

	for (const auto& [modelId, entities] : modelToEntities)
	{
		uint32_t modelBaseInstance = firstInstance;
		uint32_t entityCount = static_cast<uint32_t>(entities.size());

		ModelData& modelData = mModelData->at(modelId);

		for (uint32_t i = 0; i < modelData.mMeshCount; i++)
		{
			IndirectBuffer::DrawCommand drawCommand{};
			drawCommand.vertexOffset = static_cast<int32_t>(modelData.mVertexOffsets[i]);
			drawCommand.firstIndex = modelData.mIndexOffsets[i];
			drawCommand.indexCount = modelData.mIndexCounts[i];
			drawCommand.firstInstance = firstInstance;
			drawCommand.instanceCount = entityCount;
			mDrawCommands.emplace_back(std::move(drawCommand));

			firstInstance += entityCount;
		}

		for (uint32_t entityIdx = 0; entityIdx < entityCount; entityIdx++)
		{
			EntityId e = entities[entityIdx];

			const auto& model = mComponentManager->getComponent<ModelComponent>(e);
			const auto& transform = mComponentManager->getComponent<TransformComponent>(e);

			GPUEntity entity{};
			entity.mPosition = transform.mPosition;
			entity.mRotation = transform.mRotation;
			entity.mScale = transform.mScale;
			entity.mMeshCount = modelData.mMeshCount;
			entity.mMeshOffset = static_cast<uint32_t>(mGPUMeshes.size());
			entity.mInstanceLocalOffset = entityIdx;
			mGPUEntities.emplace_back(std::move(entity));

			for (uint32_t meshIdx = 0; meshIdx < entity.mMeshCount; meshIdx++)
			{
				const auto& meshTransform = model.mMeshTransforms[meshIdx];

				GPUMesh mesh{};
				mesh.mPosition = meshTransform.mPosition;
				mesh.mRotation = meshTransform.mRotation;
				mesh.mScale = meshTransform.mScale;
				mesh.mTransformOffset = modelData.mTransformOffsets[meshIdx];
				mesh.mTextureOffset = model.mTextureId;
				mesh.mInstanceBaseOffset = modelBaseInstance + entityCount * meshIdx;
				mGPUMeshes.emplace_back(std::move(mesh));
			}
		}
	}
}

/*
void RenderSystem::update(float deltaTime)
{
	mMeshGroups.clear();
	mGPUEntities.clear();
	mGPUMeshes.clear();

	for (EntityId e : mEntities)
	{
		const auto& model = mComponentManager->getComponent<ModelComponent>(e);
		if (model.mIsHidden) continue;

		ModelData& modelData = mModelData->at(model.mModelId);
		const auto& transform = mComponentManager->getComponent<TransformComponent>(e);

		GPUEntity entity{};
		entity.mPosition = transform.mPosition;
		entity.mRotation = transform.mRotation;
		entity.mScale = transform.mScale;
		entity.mMeshCount = modelData.mMeshCount;
		entity.mMeshIndex = static_cast<uint32_t>(mGPUMeshes.size());

		for (uint32_t i = 0; i < modelData.mMeshCount; i++)
		{
			const auto& meshTransform = model.mMeshTransforms[i];

			GPUMesh mesh{};
			mesh.mPosition = meshTransform.mPosition;
			mesh.mRotation = meshTransform.mRotation;
			mesh.mScale = meshTransform.mScale;
			mesh.mTransformIndex = modelData.mTransformOffsets[i];
			mesh.mTextureIndex = model.mTextureId;
			mesh.mDrawIndex = static_cast<uint32_t>(mGPUMeshes.size());
			mGPUMeshes.emplace_back(std::move(mesh));
		}

		mGPUEntities.emplace_back(std::move(entity));
	}

	for (EntityId e : mEntities)
	{
		const auto& model = mComponentManager->getComponent<ModelComponent>(e);
		if (model.mIsHidden) continue;

		ModelData& modelData = mModelData->at(model.mModelId);
		const auto& transform = mComponentManager->getComponent<TransformComponent>(e);

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

			// instance.mModelTransform = modelMatrix;
			// instance.mMeshTransform = meshMatrix;

			// instance.mTransformId = modelData.mTransformOffsets[i];
			// instance.mTextureId = model.mTextureId;

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
*/

void RenderSystem::updateModels(std::unordered_map<uint32_t, ModelData>& modelData)
{
	mModelData = &modelData;
}

const std::vector<GPUEntity>& RenderSystem::getEntities() const
{
	return mGPUEntities;
}

const std::vector<GPUMesh>& RenderSystem::getMeshes() const
{
	return mGPUMeshes;
}

const std::vector<IndirectBuffer::DrawCommand> RenderSystem::getDrawCommands() const
{
	return mDrawCommands;
}