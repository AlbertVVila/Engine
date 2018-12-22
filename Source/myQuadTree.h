#ifndef __myQuadTree_h__
#define __myQuadTree_h__

#include "Geometry/AABB.h"
#include <vector>

class GameObject;

struct Node
{
	/// If 0, this node is the root.
	Node *parent;
	/// Indicates the quad of child nodes for this node, or 0xFFFFFFFF if this node is a leaf.
	unsigned childIndex;
	/// Stores the actual objects in this node/leaf.
	std::vector<GameObject*> objects;
}

class myQuadTree
{
public:
	myQuadTree();
	~myQuadTree();

	void Create(AABB limits) : limits(limits) {};

	void Clear();
	void Insert(GameObject* gameobject);
	//CollectingIntersect(std::vector<GameObject*>&, );
	
	unsigned bucketSize = 4;
private:
	std::vector<Node*> nodes;
	AABB limits;
};

#endif __myQuadTree_h__
