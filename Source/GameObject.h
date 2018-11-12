#ifndef __GameObject_h__
#define __GameObject_h__

#include <vector>

class Component;
class ComponentTransform;

class GameObject
{
public:
	GameObject();
	~GameObject();

	void Update();

public:
	ComponentTransform* transform = nullptr;
	std::vector<Component*> components;
};

#endif __GameObject_h__

