#include "ModuleSpacePartitioning.h"
#include "Application.h"


bool ModuleSpacePartitioning::Init() 
{
	kDTree.Init();
	aabbTree.Init(GameObject::GameObjectLayers::WORLD_VOLUME);
	aabbTreeLighting.Init(GameObject::GameObjectLayers::LIGHTING);
	return true;
}

update_status ModuleSpacePartitioning::Update()
{		
	return UPDATE_CONTINUE;
}

bool ModuleSpacePartitioning::CleanUp()
{
	aabbTree.CleanUp();
	return true;
}


