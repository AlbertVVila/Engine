#include "AABBTree.h"



AABBTree::AABBTree()
{
}


AABBTree::~AABBTree()
{
}

int AABBTree::AllocateNode()
{
	int index = nodes.size();
	AABBNode *n = new AABBNode();
	nodes.push_back(n);
	return index;
}

void AABBTree::Insert(GameObject * gameobject)
{
	unsigned index = AllocateNode(); //new node
	AABBNode *node = nodes[index];
	node->aabb = gameobject->GetBoundingBox();

	if (rootIndex == -1 || nodes[rootIndex] == nullptr)
	{
		rootIndex = index;
	}
	else if (nodes[rootIndex]->IsLeaf())
	{

	}
	//leaf->childIndex = AllocateNode(leaf);

}
