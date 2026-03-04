#include "DescriptorRegistry.h"

#include "../Resource/ResourceRegistryBackend.h"
#include "../../Resource/Buffer/Buffer.h"
#include "../../Resource/Texture/Texture.h"
#include "../../Resource/Sampler/Sampler.h"

#include <stdexcept>

using namespace ascen;

DescriptorRegistry::DescriptorRegistry(
	VulkanContext& context,
	ResourceRegistry& resourceRegistry) :
	mDevice(context.getDevice()),
	mDescriptorFactory(context.getDescriptorFactory()),
	mResourceRegistry(resourceRegistry) {}

void DescriptorRegistry::registerDescriptor(DescriptorSetEntry entry)
{
	mDescriptorPoolRequirements[entry.mType]++;
	const std::string& setName = entry.mSetName;
	mSetEntries[setName].emplace_back(std::move(entry));
}

void DescriptorRegistry::reconstruct()
{
	std::vector<DescriptorPool::Size> sizes;

	for (auto& [type, count] : mDescriptorPoolRequirements)
	{
		if (count > 0)
		{
			DescriptorPool::Size size{};
			size.type = static_cast<VkDescriptorType>(type);
			size.descriptorCount = count;
			sizes.push_back(size);
		}
	}

	mDescriptorPool = mDescriptorFactory.createPool(sizes);
	
	for (auto& [name, entries] : mSetEntries)
	{
		if (mDescriptorSetLayouts.find(name) == mDescriptorSetLayouts.end())
		{
			std::vector<DescriptorSetLayout::Binding> bindings;
			std::vector<DescriptorSet::Write> writes;

			for (auto& entry : entries)
			{
				bindings.push_back(mDescriptorFactory.createBinding(
					entry.mSlot,
					static_cast<VkDescriptorType>(entry.mType),
					static_cast<uint32_t>(entry.mStage)));

				switch (entry.mType)
				{
				case DescriptorType::IMAGE:
				{
					const TexturePtr& texture = ResourceRegistryBackend::getTexture(
						mResourceRegistry, entry.mResourceName);

					writes.push_back(mDescriptorFactory.createImageWrite(
						static_cast<VkDescriptorType>(entry.mType),
						texture->handle(),
						VK_NULL_HANDLE,
						entry.mSlot));
					break;
				}
				case DescriptorType::SAMPLER:
				{
					const SamplerPtr& sampler = ResourceRegistryBackend::getSampler(
						mResourceRegistry, entry.mResourceName);

					writes.push_back(mDescriptorFactory.createImageWrite(
						static_cast<VkDescriptorType>(entry.mType),
						VK_NULL_HANDLE,
						sampler->handle(),
						entry.mSlot));
					break;
				}
				case DescriptorType::IMAGE_SAMPLER:
					break;
				case DescriptorType::UBO:
				case DescriptorType::UBO_DYNAMIC:
				case DescriptorType::SSBO:
				case DescriptorType::SSBO_DYNAMIC:
				default:
				{
					const BufferPtr& buffer = ResourceRegistryBackend::getBuffer(
						mResourceRegistry, entry.mResourceName);

					writes.push_back(mDescriptorFactory.createBufferWrite(
						static_cast<VkDescriptorType>(entry.mType),
						buffer->handle(),
						0,
						static_cast<VkDeviceSize>(entry.mSize),
						entry.mSlot));
				}
				}
			}

			mDescriptorSetLayouts.emplace(name, mDescriptorFactory.createSetLayout(bindings));
			mDescriptorSets.emplace(name, mDescriptorFactory.createSet(
				mDescriptorPool, mDescriptorSetLayouts.at(name), writes));
		}
		else
		{
			throw std::runtime_error("A descriptor set layout with that name already exists!");
		}
	}
	
}

void DescriptorRegistry::cleanup()
{
	for (auto& [name, layout] : mDescriptorSetLayouts)
	{
		if (layout) layout->destroy(mDevice);
	}
	mDescriptorSetLayouts.clear();
	mDescriptorSets.clear();

	mDescriptorPool->destroy(mDevice);
}

bool DescriptorRegistry::exists(const std::string& name) const
{
	return (mDescriptorSetLayouts.find(name) != mDescriptorSetLayouts.end());
}
