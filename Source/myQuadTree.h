#ifndef __myQuadTree_h__
#define __myQuadTree_h__

#include "Geometry/AABB.h"
#include <vector>

class GameObject;

struct Node
{
	Node *parent;
	unsigned childIndex;

	bool IsLeaf() const { return childIndex == 0xFFFFFFFF; }

	unsigned TopLeftChildIndex() const { return childIndex; }
	unsigned TopRightChildIndex() const { return childIndex + 1; }
	unsigned BottomLeftChildIndex() const { return childIndex + 2; }
	unsigned BottomRightChildIndex() const { return childIndex + 3; }

	std::vector<const GameObject*> gameobjects;

	void Remove(const GameObject& gameobject)
	{
		for (unsigned i = 0; i < gameobjects.size(); i++)
		{
			if (&gameobject == gameobjects[i])
			{

			}
		}
	}
};

class myQuadTree
{
public:
	myQuadTree();
	~myQuadTree();

	myQuadTree(AABB limits);

	void Clear();
	void Insert(const GameObject& gameobject);
	void Add(const GameObject & gameobject, Node * node, AABB boundingBox);
	void Split(Node * leaf, AABB leafAABB);
	int AllocateNode(Node * parent);
	void Remove(const GameObject& gameobject);
	//CollectingIntersect(std::vector<GameObject*>&, );
	
	unsigned bucketSize = 4;
private:
	std::vector<Node*> nodes;
	AABB limits;
	unsigned rootIndex = -1;
};

#endif __myQuadTree_h__
