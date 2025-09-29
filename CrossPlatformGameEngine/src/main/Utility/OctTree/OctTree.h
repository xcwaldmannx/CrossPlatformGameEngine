#pragma once

#include <memory>

#include <array>
#include <vector>

#include <glm/glm.hpp>

struct OctTreeRegion
{
	bool contains(glm::vec3 position) const
	{
		return position.x >= mPosition.x &&
			   position.y >= mPosition.y &&
			   position.z >= mPosition.z &&
			   position.x < mPosition.x + mSize &&
			   position.y < mPosition.y + mSize &&
			   position.z < mPosition.z + mSize;
	}

	bool contains(const OctTreeRegion& region) const
	{
		return region.mPosition.x >= mPosition.x &&
			   region.mPosition.y >= mPosition.y &&
			   region.mPosition.z >= mPosition.z &&
			   region.mPosition.x + region.mSize < mPosition.x + mSize &&
			   region.mPosition.y + region.mSize < mPosition.y + mSize &&
			   region.mPosition.z + region.mSize < mPosition.z + mSize;
	}

	bool intersects(const OctTreeRegion& region) const
	{
		return region.mPosition.x < mPosition.x + mSize &&
			   region.mPosition.y < mPosition.y + mSize &&
			   region.mPosition.z < mPosition.z + mSize &&
			   region.mPosition.x + region.mSize > mPosition.x &&
			   region.mPosition.y + region.mSize > mPosition.y &&
			   region.mPosition.z + region.mSize > mPosition.z;
	}

	glm::vec3 mPosition;
	unsigned int mSize;
};

template<typename T>
class OctTreeNode
{
public:
	OctTreeNode(OctTreeRegion region, unsigned int depth) : mRegion(region), mDepth(depth) 
	{
		if (mDepth == 0)
		{
			mIsLeaf = true;
		}
	}

	void insert(const T& item, const glm::vec3& position)
	{
		if (mRegion.contains(position))
		{
			OctTreeNode<T>* node = findNode(position);
			node->mItems.push_back(item);
		}
	}

	std::vector<T> search(const OctTreeRegion& region) const
	{
		std::vector<T> items;

		if (region.contains(mRegion))
		{
			// return all items, no need to keep searching children
			const auto& allItems = getAllItems();
			items.insert(items.end(), allItems.begin(), allItems.end());
			return items;
		}
		else if (region.intersects(mRegion))
		{
			searchNode(this, region, &items);
		}

		return items;
	}

	std::vector<T> getAllItems() const
	{
		std::vector<T> items;

		getAllItems(this, &items);

		return items;
	}

private:
	static void searchNode(const OctTreeNode<T>* node, const OctTreeRegion& region, std::vector<T>* items)
	{
		if (node->mIsLeaf)
		{
			const auto& allItems = node->getAllItems();
			items->insert(items->end(), allItems.begin(), allItems.end());
		}
		else
		{
			for (const auto& child : node->mNodes)
			{
				if (!child) continue;

				if (region.contains(child->mRegion))
				{
					// return all items, no need to keep searching children
					const auto& allItems = child->getAllItems();
					items->insert(items->end(), allItems.begin(), allItems.end());
					continue;
				}
				else if (region.intersects(child->mRegion))
				{
					searchNode(child.get(), region, items);
				}
			}
		}

	}

	static void getAllItems(const OctTreeNode<T>* node, std::vector<T>* items)
	{
		if (node->mIsLeaf)
		{
			items->insert(items->end(), node->mItems.begin(), node->mItems.end());
		}
		else
		{
			for (const auto& child : node->mNodes)
			{
				getAllItems(child.get(), items);
			}
		}
	}

	OctTreeNode<T>* findNode(const glm::vec3& position)
	{
		// N0 = { 0, 0, 0 }
		// N1 = { 0, 0, 1 }
		// N2 = { 0, 1, 0 }
		// N3 = { 0, 1, 1 }
		// N4 = { 1, 0, 0 }
		// N5 = { 1, 0, 1 }
		// N6 = { 1, 1, 0 }
		// N7 = { 1, 1, 1 }

		// Bottom  Top
		// N2 N6   N3 N7
		// N0 N4   N1 N5

		if (mDepth > 0)
		{
			unsigned int node = 0;

			for (int x = 0; x < 2; x++)
			{
				for (int y = 0; y < 2; y++)
				{
					for (int z = 0; z < 2; z++)
					{
						OctTreeRegion region{};

						region.mPosition = {
							mRegion.mPosition.x + (x * mRegion.mSize / 2),
							mRegion.mPosition.y + (y * mRegion.mSize / 2),
							mRegion.mPosition.z + (z * mRegion.mSize / 2)
						};

						region.mSize = mRegion.mSize / 2;

						if (region.contains(position))
						{
							if (!mNodes[node])
							{
								mNodes[node] = std::make_unique<OctTreeNode<T>>(region, mDepth - 1);
							}

							return mNodes[node]->findNode(position);
						}

						node++;
					}
				}
			}
		}
		else
		{
			return this;
		}
	}

private:
	OctTreeRegion mRegion{ { 0, 0, 0 }, 0 };
	unsigned int mDepth = 0;
	bool mIsLeaf = false;
	std::array<std::unique_ptr<OctTreeNode<T>>, 8> mNodes;
	std::vector<T> mItems;
};

template<typename T>
using OctTree = OctTreeNode<T>;
