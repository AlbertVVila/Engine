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
	Node * rootNode;
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
	float2 half = float2((bbox.minPoint.x + bbox.maxPoint.x), (bbox.minPoint.y + bbox.maxPoint.y))*0.5f;
	AABB goBBox = gameobject.GetBoundingBox();

	if (node->IsLeaf()) //TODO: does not check if go is inside node bbox
	{
		node->gameobjects.push_back(&gameobject);
		if (node->gameobjects.size() > bucketSize)
		{
			Split(node, bbox);
		}
	}

	bool left = goBBox.minPoint.x < half.x;
	bool right = goBBox.maxPoint.x > half.x;
	bool top = goBBox.minPoint.y < half.y;
	bool bot = goBBox.maxPoint.y > half.y;
}

void myQuadTree::Split(Node* leaf, AABB leafAABB)
{
	assert(leaf->IsLeaf());
	leaf->childIndex = AllocateNode(leaf);
	//Loop insert on children and popback from old node
}

int myQuadTree::AllocateNode(Node *parent)
{
	int index = nodes.size();
	for (unsigned i = 0; i < 4; i++)
	{
		Node *n = new Node();
		n->parent = parent;
		n->childIndex = 0XFFFFFFFF;
		nodes.push_back(n);
	}
	return index;
}