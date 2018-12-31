#ifndef __AABBTree_h__
#define __AABBTree_h__

#include <vector>
#include "Geometry/AABB.h"
#include "GameObject.h"

struct AABBNode
{
	AABB aabb;

	AABBNode* parent = nullptr;

	unsigned leftChild = 0xFFFFFFFF; 
	unsigned rightChild = 0xFFFFFFFF;

	unsigned height = 0;

	bool IsLeaf()
	{
		return leftChild == 0xFFFFFFFF;
	}

	std::vector<GameObject*> gameobjects;
};

class AABBTree
{
public:
	AABBTree();
	~AABBTree();

	int AllocateNode();
	void Insert(GameObject* gameobject);
	void Remove(const GameObject& gameobject);

	std::vector<AABBNode*> nodes;
	unsigned rootIndex = -1;
};

#endif __AABBTree_h__