#pragma once

#include "../../FrameGraph/FramePass/FramePass.h"
#include "../../FrameGraph/FramePass/GpuFramePass/GraphicsGpuFramePass/GraphicsGpuFramePass.h"
#include "../../FrameGraph/FramePass/GpuFramePass/ComputeGpuFramePass/ComputeGpuFramePass.h"
#include "../../FrameGraph/FramePass/SyncFramePass/SyncFramePass.h"

#include "../Resource/ResourceRegistry.h"
#include "../Descriptor/DescriptorRegistry.h"
#include "../Pipeline/PipelineRegistry.h"

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

namespace ascen
{

	struct FramePassEntry
	{
		std::string mName;
		std::vector<std::string> mDescriptorSets;
		std::string mPipeline;
		std::function<void()> mPreProcess;
		std::function<void()> mPostProcess;
	};

	struct GpuFramePassEntry : FramePassEntry
	{
		std::vector<GpuResource> mResources;
	};

	struct GraphicsFramePassEntry : GpuFramePassEntry
	{
		GraphicsMode mFramePassMode = GraphicsMode::MESH;
	};

	struct ComputeFramePassEntry : GpuFramePassEntry
	{
		std::array<uint32_t, 3> mGroups{ 1, 1, 1 };
	};

	struct SyncFramePassEntry
	{
		std::string mName;
		VkBuffer mBuffer;
		VkAccessFlags2 mSrcAccess;
		VkPipelineStageFlags2 mSrcStage;
		VkAccessFlags2 mDstAccess;
		VkPipelineStageFlags2 mDstStage;
		std::function<void()> mPreProcess;
		std::function<void()> mPostProcess;
	};

	class FramePassRegistry : public Registry_I
	{
	public:
		void registerGraphics(GraphicsFramePassEntry entry);
		void registerCompute(ComputeFramePassEntry entry);
		void registerSync(SyncFramePassEntry entry);

		void reconstruct() override;

		void cleanup();

	private:
		bool isRegistered(const std::string& name) const;
		bool exists(const std::string& name) const;

	private:
		std::vector<std::string> mRegisteredNames;

		std::vector<GraphicsFramePassEntry> mGraphicsEntries;
		std::vector<ComputeFramePassEntry> mComputeEntries;
		std::vector<SyncFramePassEntry> mSyncEntries;

		std::unordered_map<std::string, FramePassPtr> mFramePasses;

		friend class FramePassRegistryBackend;
	};

}
