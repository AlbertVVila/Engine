#include "myQuadTree.h"
#include "GameObject.h"


myQuadTree::myQuadTree()
{
}


myQuadTree::~myQuadTree()
{
}

void myQuadTree::Clear()
{
}

void myQuadTree::Insert(GameObject * gameobject)
{
	if (limits.Contains(gameobject->GetBoundingBox()))
	{

	}
	//Add to rootnode
	//if you run out of bucket space and you are a leaf, subdivide yourself in 4
	//add it node's list
	//redistribute gameobjects to proper childs based on their positin in space
}
