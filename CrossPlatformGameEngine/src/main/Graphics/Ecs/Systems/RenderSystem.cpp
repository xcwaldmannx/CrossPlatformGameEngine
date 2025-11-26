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