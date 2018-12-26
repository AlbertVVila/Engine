#ifndef __myQuadTree_h__
#define __myQuadTree_h__

#include "Geometry/AABB.h"
#include <vector>

enum class QUADRANT
{
	TL = 0, //Top Left
	TR,
	BL,
	BR
};


class GameObject;

struct Node
{
	Node *parent = nullptr;
	unsigned depth = 0;
	unsigned childIndex = 0xFFFFFFFF; //Leaf by default

	bool IsLeaf() const { return childIndex == 0xFFFFFFFF; }

	unsigned TopLeftChildIndex() const { return childIndex; }
	unsigned TopRightChildIndex() const { return childIndex + 1; }
	unsigned BottomLeftChildIndex() const { return childIndex + 2; }
	unsigned BottomRightChildIndex() const { return childIndex + 3; }

	std::vector<const GameObject*> gameobjects;

	bool Remove(const GameObject& gameobject)
	{
		for (std::vector<const GameObject*>::const_iterator it = gameobjects.begin(); it != gameobjects.end(); ++it)
		{
			if (*it == &gameobject)
			{
				gameobjects.erase(it);
				return true;
			}
		}
		return false;
	}
};

class myQuadTree
{
public:
	myQuadTree(AABB limits);
	~myQuadTree();

	void Clear();
	void Insert(const GameObject& gameobject);
	void Add(const GameObject & gameobject, Node * node, AABB boundingBox);
	void Split(Node * leaf, AABB leafAABB);
	void Remove(const GameObject& gameobject);

	int AllocateNode(Node * parent);
	void Draw();
	void Draw(AABB bbox);
	void ExtendLimitTopLeft();
	void ExtendLimitTopRight();
	void ExtendLimitBotLeft();
	void ExtendLimitBotRight();
	void RecomputeRoot(QUADRANT q);
	AABB GetBoundingBox(const Node *node) const;
	//CollectingIntersect(std::vector<GameObject*>&, );

public:
	unsigned bucketSize = 2;
	unsigned maxDepth = 4;
private:
	std::vector<Node*> nodes;
	AABB limits;
	unsigned rootIndex = -1;
};

#endif __myQuadTree_h__
