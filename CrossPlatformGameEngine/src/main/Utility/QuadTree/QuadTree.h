#pragma once

#include <memory>

#include <array>
#include <vector>

#include <glm/glm.hpp>

//TODO: do not instantiate every leaf node on creation, only as elements are added

struct QuadTreeRegion
{
	bool contains(glm::vec2 position) const
	{
		return position.x >= mPosition.x &&
			   position.y >= mPosition.y &&
			   position.x < mPosition.x + mSize &&
			   position.y < mPosition.y + mSize;
	}

	bool contains(const QuadTreeRegion& region) const
	{
		return region.mPosition.x >= mPosition.x &&
			   region.mPosition.y >= mPosition.y &&
			   region.mPosition.x + region.mSize < mPosition.x + mSize &&
			   region.mPosition.y + region.mSize < mPosition.y + mSize;
	}

	bool intersects(const QuadTreeRegion& region) const
	{
		return region.mPosition.x < mPosition.x + mSize &&
			   region.mPosition.y < mPosition.y + mSize &&
			   region.mPosition.x + region.mSize > mPosition.x &&
			   region.mPosition.y + region.mSize > mPosition.y;
	}

	glm::vec2 mPosition;
	unsigned int mSize;
};

template<typename T>
class QuadTreeNode
{
public:
	QuadTreeNode(glm::vec2 position, unsigned int size, unsigned int depth)
		: mRegion(position, size), mDepth(depth)
	{
		unsigned int halfSize = size / 2;

		mNodes[0] = std::make_unique<QuadTreeNode<T>>(position, halfSize, 0, mDepth);
		mNodes[1] = std::make_unique<QuadTreeNode<T>>(position + glm::vec2(halfSize, 0), halfSize, 0, mDepth);
		mNodes[2] = std::make_unique<QuadTreeNode<T>>(position + glm::vec2(0, halfSize), halfSize, 0, mDepth);
		mNodes[3] = std::make_unique<QuadTreeNode<T>>(position + glm::vec2(halfSize, halfSize), halfSize, 0, mDepth);
	}

	QuadTreeNode(glm::vec2 position, unsigned int size, unsigned int depth, unsigned int maxDepth)
		: mRegion(position, size), mDepth(depth)
	{
		// node position order
		// 2, 3
		// 0, 1

		if (depth != maxDepth)
		{
			unsigned int halfSize = size / 2;
			unsigned int newDepth = depth + 1;

			mNodes[0] = std::make_unique<QuadTreeNode<T>>(position, halfSize, newDepth, maxDepth);
			mNodes[1] = std::make_unique<QuadTreeNode<T>>(position + glm::vec2(halfSize, 0), halfSize, newDepth, maxDepth);
			mNodes[2] = std::make_unique<QuadTreeNode<T>>(position + glm::vec2(0, halfSize), halfSize, newDepth, maxDepth);
			mNodes[3] = std::make_unique<QuadTreeNode<T>>(position + glm::vec2(halfSize, halfSize), halfSize, newDepth, maxDepth);
		}
		else
		{
			mIsLeaf = true;
		}
	}

public:
	void insert(const T& item, const glm::vec2 position)
	{
		if (mRegion.contains(position))
		{
			insert(item, position, this);
		}
	}

private:
	static void insert(const T& item, const glm::vec2& position, QuadTreeNode<T>* node)
	{
		if (node->mIsLeaf)
		{
			node->mItems.push_back(item);
		}
		else
		{
			for (auto& child : node->mNodes)
			{
				if (child->mRegion.contains(position))
				{
					insert(item, position, child.get());
					break;
				}
			}
		}
	}

public:
	std::vector<T> search(const QuadTreeRegion& region) const
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

private:
	static void searchNode(const QuadTreeNode<T>* node, const QuadTreeRegion& region, std::vector<T>* items)
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

public:
	std::vector<T> getAllItems() const
	{
		std::vector<T> items;

		getAllItems(this, &items);

		return items;
	}

private:
	static void getAllItems(const QuadTreeNode<T>* node, std::vector<T>* items)
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

private:
	QuadTreeRegion mRegion{ { 0, 0 }, 0 };
	unsigned int mDepth = 0;
	bool mIsLeaf = false;
	std::array<std::unique_ptr<QuadTreeNode<T>>, 4> mNodes;
	std::vector<T> mItems;
};

template<typename T>
using QuadTree = QuadTreeNode<T>;
