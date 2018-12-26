#include "myQuadTree.h"
#include "GameObject.h"
#include "Math/float2.h"


myQuadTree::myQuadTree()
{
}


myQuadTree::~myQuadTree()
{
}

myQuadTree::myQuadTree(AABB limits) : limits(limits)
{
	Node * rootNode = new Node();
	rootIndex = 0;
	nodes.push_back(rootNode);
}

void myQuadTree::Clear()
{
}

void myQuadTree::Insert(const GameObject& gameobject) //TODO: make it adaptative
{
	AABB bbox = gameobject.GetBoundingBox();
	if (limits.Contains(bbox))
	{
		Add(gameobject, nodes[rootIndex], limits);
	}
}

void myQuadTree::Add(const GameObject& gameobject, Node* node, AABB bbox)
{
	while (true) //should look depth
	{
		float2 half = float2((bbox.minPoint.x + bbox.maxPoint.x), (bbox.minPoint.z + bbox.maxPoint.z))*0.5f;
		AABB goBBox = gameobject.GetBoundingBox();

		bool left = goBBox.minPoint.x < half.x;
		bool right = goBBox.maxPoint.x > half.x;
		bool top = goBBox.minPoint.z < half.y;
		bool bot = goBBox.maxPoint.z > half.y;

		if (node->IsLeaf() || (left && right) || (top && bot))
		{
			node->gameobjects.push_back(&gameobject);
			if (node->gameobjects.size() > bucketSize && node->IsLeaf())
			{
				if (node->depth < maxDepth)
				{
					Split(node, bbox);
				}
			}
			return;
		}
		//iterate over children nodes
		if (left)
		{
			bbox.maxPoint.x = half.x;
			if (top)
			{
				bbox.maxPoint.z = half.y;
				node = nodes[node->TopLeftChildIndex()];
			}
			else //bot
			{
				bbox.minPoint.z = half.y;
				node = nodes[node->BottomLeftChildIndex()];
			}
		}
		else //right
		{
			bbox.minPoint.x = half.x;
			if (top)
			{
				bbox.maxPoint.z = half.y;
				node = nodes[node->TopRightChildIndex()];
			}
			else
			{
				bbox.minPoint.z = half.y;
				node = nodes[node->BottomRightChildIndex()];
			}
		}
	}
}

void myQuadTree::Split(Node* leaf, AABB leafAABB)
{
	assert(leaf->IsLeaf());
	leaf->childIndex = AllocateNode(leaf);
	float2 half = float2((leafAABB.minPoint.x + leafAABB.maxPoint.x), 
		(leafAABB.minPoint.z + leafAABB.maxPoint.z))*0.5f;
	
	unsigned i = 0;
	while (i < leaf->gameobjects.size())
	{
		const GameObject &go = *leaf->gameobjects[i];
		AABB bbox = go.GetBoundingBox();

		bool left = bbox.minPoint.x < half.x;
		bool right = bbox.maxPoint.x > half.x;
		bool top = bbox.minPoint.z < half.y;
		bool bot = bbox.maxPoint.z > half.y;
		
		if ((left && right) || (top && bot))
		{
			++i;
			continue;
		}

		AABB child;
		child.minPoint.y = leafAABB.minPoint.y; //Always the same because we are in a quadtree (2D)
		child.maxPoint.y = leafAABB.maxPoint.y;
		if (left)
		{
			child.minPoint.x = leafAABB.minPoint.x;
			child.maxPoint.x = half.x;
			if (top)
			{
				child.minPoint.z = leafAABB.minPoint.z;
				child.maxPoint.z = half.y;//halfz
				Add(go, nodes[leaf->TopLeftChildIndex()], child);
			}
			else //bot
			{
				child.minPoint.z = half.y;
				child.maxPoint.z = leafAABB.maxPoint.z;
				Add(go, nodes[leaf->BottomLeftChildIndex()], child);
			}
		}
		else //right
		{
			child.minPoint.x = half.x;
			child.maxPoint.x = leafAABB.maxPoint.x;
			if (top)
			{
				child.minPoint.z = leafAABB.minPoint.z;
				child.maxPoint.z = half.y;
				Add(go, nodes[leaf->TopRightChildIndex()], child);
			}
			else
			{
				child.minPoint.z = half.y;
				child.maxPoint.z = leafAABB.maxPoint.z;
				Add(go, nodes[leaf->BottomRightChildIndex()], child);
			}
		}
		std::swap(leaf->gameobjects[i], leaf->gameobjects.back());
		leaf->gameobjects.pop_back();
	}
}

int myQuadTree::AllocateNode(Node *parent)
{
	int index = nodes.size();
	for (unsigned i = 0; i < 4; i++)
	{
		Node *n = new Node();
		n->parent = parent;
		n->depth = parent->depth + 1;
		n->childIndex = 0XFFFFFFFF;
		nodes.push_back(n);
	}
	return index;
}