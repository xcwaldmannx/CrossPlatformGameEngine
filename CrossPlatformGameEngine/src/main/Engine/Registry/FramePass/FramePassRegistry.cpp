#include "FramePassRegistry.h"

#include <stdexcept>

#include "../../FrameGraph/FramePass/GpuFramePass/GraphicsGpuFramePass/GraphicsGpuFramePass.h"
#include "../../FrameGraph/FramePass/GpuFramePass/ComputeGpuFramePass/ComputeGpuFramePass.h"
#include "../Resource/ResourceRegistryBackend.h"
#include "../Descriptor/DescriptorRegistryBackend.h"
#include "../Pipeline/PipelineRegistryBackend.h"
#include "../../Resource/Buffer/Buffer.h"
#include "../../Resource/Texture/Texture.h"
#include "../../Pipeline/GraphicsPipeline/GraphicsPipeline_I.h"
#include "../../Pipeline/ComputePipeline/ComputePipeline_I.h"

using namespace ascen;

void FramePassRegistry::registerGraphics(GraphicsFramePassEntry entry)
{
	if (isRegistered(entry.mName))
	{
		throw std::runtime_error("A frame pass with that name already exists!");
	}

	mGraphicsEntries.emplace_back(std::move(entry));
}

void FramePassRegistry::registerCompute(ComputeFramePassEntry entry)
{
	if (isRegistered(entry.mName))
	{
		throw std::runtime_error("A frame pass with that name already exists!");
	}

	mComputeEntries.emplace_back(std::move(entry));
}

void FramePassRegistry::registerSync(SyncFramePassEntry entry)
{
	if (isRegistered(entry.mName))
	{
		throw std::runtime_error("A frame pass with that name already exists!");
	}

	mSyncEntries.emplace_back(std::move(entry));
}

void FramePassRegistry::reconstruct()
{
	cleanup();

	for (const auto& entry : mGraphicsEntries)
	{
		mFramePasses[entry.mName] = std::make_shared<GraphicsGpuFramePass>(
			FramePassType::GRAPHICS,
			entry.mFramePassMode,
			entry.mPipeline,
			entry.mDescriptorSets,
			entry.mResources);
	}

	for (const auto& entry : mComputeEntries)
	{
		mFramePasses[entry.mName] = std::make_shared<ComputeGpuFramePass>(
			FramePassType::COMPUTE,
			entry.mPipeline,
			entry.mDescriptorSets,
			entry.mResources,
			entry.mGroups);
	}

	for (const auto& entry : mSyncEntries)
	{
		mFramePasses[entry.mName] = std::make_shared<SyncFramePass>(
			FramePassType::SYNC,
			entry.mBuffer,
			entry.mSrcAccess,
			entry.mSrcStage,
			entry.mDstAccess,
			entry.mDstStage);
	}
}

void FramePassRegistry::cleanup()
{
	mFramePasses.clear();
}

bool FramePassRegistry::isRegistered(const std::string& name) const
{
	for (const auto& registeredName : mRegisteredNames)
	{
		if (name == registeredName)
		{
			return true;
		}
	}

	return false;
}

bool FramePassRegistry::exists(const std::string& name) const
{
	return (mFramePasses.contains(name));
}
