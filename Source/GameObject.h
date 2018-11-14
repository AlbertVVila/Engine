#ifndef __GameObject_h__
#define __GameObject_h__

#include <vector>
#include "assimp/matrix4x4.h"

class Component;
class ComponentTransform;
enum ComponentType;
struct Texture;

class GameObject
{
public:
	GameObject();
	GameObject(const aiMatrix4x4& transform);
	~GameObject();
	void Draw();

	void Update();
	Component * CreateComponent(ComponentType type);

	Texture * GetMaterialTexture() const;

public:
	//ComponentTransform* transform = nullptr;
	std::vector<Component*> components;
};

#endif __GameObject_h__

