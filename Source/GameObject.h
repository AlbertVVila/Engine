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
	GameObject(const aiMatrix4x4& transform, const char* filepath, const char* name);
	~GameObject();
	void Draw();
	void DrawProperties();
	void SetParent(GameObject* parent);
	void Update();
	Component * CreateComponent(ComponentType type);
	Component * GetComponent(ComponentType type);
	std::vector<Component *> GetComponents(ComponentType type);
	std::string GetFileFolder() const;

private:
	void ModelTransform(unsigned int shader) const;

public:
	ComponentTransform * transform = nullptr;
	GameObject *parent = nullptr;
	std::vector<Component*> components;
	std::vector<GameObject*> children;
	const char * filepath = nullptr; // Change to string 
	std::string name = "GameObject";
};

#endif __GameObject_h__

