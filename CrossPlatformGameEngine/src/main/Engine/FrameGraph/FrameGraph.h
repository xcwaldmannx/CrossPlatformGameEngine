#pragma once

#include "../Core/Types.h"
#include "FramePass/FramePass.h"

#include <vector>
#include <queue>
#include <stdexcept>
#include <sstream>

#include <vulkan/vulkan.h>

namespace ascen
{

	/*
	struct ResourceLastUsage
	{
		GpuResource mResource;
		unsigned int mLastUsageNode;
	};

	struct FrameGraphNode
	{
		std::string mName;
		FramePassPtr mFramePass;
		std::vector<unsigned int> mDependencies;
		std::vector<GpuResource> mResources;
	};
	*/

	class FrameGraph
	{
	public:
		std::vector<FramePassPtr> compile(const std::vector<FramePassPtr>& framePasses);

	private:
		// bool needsBarrier(const GpuResource& previous, const GpuResource& current);

		// Returns a list of pass indices in execution order.
		/*
		static std::vector<unsigned int> topoSort(const std::vector<FrameGraphNode>& nodes)
		{
			const unsigned int n = static_cast<unsigned int>(nodes.size());

			// indegree[i] = number of prerequisites
			std::vector<unsigned int> indegree(n, 0);

			// Build reverse adjacency: for each node u, who depends on u?
			std::vector<std::vector<unsigned int>> dependents(n);

			for (unsigned int i = 0; i < n; ++i)
			{
				indegree[i] = static_cast<unsigned int>(nodes[i].mDependencies.size());

				for (unsigned int dep : nodes[i].mDependencies)
				{
					if (dep >= n)
						throw std::runtime_error("FrameGraph topoSort: dependency index out of range");

					dependents[dep].push_back(i);
				}
			}

			// Start with all nodes that have no dependencies
			std::queue<unsigned int> q;
			for (unsigned int i = 0; i < n; ++i)
			{
				if (indegree[i] == 0)
					q.push(i);
			}

			std::vector<unsigned int> order;
			order.reserve(n);

			while (!q.empty())
			{
				unsigned int u = q.front();
				q.pop();

				order.push_back(u);

				// "Remove" u: decrement indegree of its dependents
				for (unsigned int v : dependents[u])
				{
					if (--indegree[v] == 0)
						q.push(v);
				}
			}

			// Cycle detection
			if (order.size() != n)
			{
				// Gather remaining nodes with indegree > 0 for debugging
				std::ostringstream oss;
				oss << "FrameGraph topoSort: cycle detected. Remaining nodes:";
				for (unsigned int i = 0; i < n; ++i)
				{
					if (indegree[i] > 0)
						oss << " " << i;
				}
				throw std::runtime_error(oss.str());
			}

			return order;
		}
		*/

	private:
		// static std::unordered_map<ResourceAccess, VkAccessFlags2> sResourceAccessMap;
		// static std::unordered_map<ResourceStage, VkPipelineStageFlags2> sResourceStageMap;
	};

}
