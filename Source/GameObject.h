#ifndef __GameObject_h__
#define __GameObject_h__

#include <vector>
#include <list>
#include "Math/float4x4.h"

class Component;
class ComponentTransform;
class Frustum;
enum ComponentType;
struct Texture;

class GameObject
{
public:
	GameObject(const char* name, unsigned uuid);
	GameObject(const float4x4& transform, const char* filepath, const char* name, unsigned uuid);
	GameObject(const GameObject& gameobject);
	~GameObject();

	void Draw(const math::Frustum& frustum);
	void DrawProperties();
	void DrawHierarchy(GameObject * obj_clicked);

	void Update();

	Component * CreateComponent(ComponentType type);
	Component * GetComponent(ComponentType type) const;
	std::vector<Component *> GetComponents(ComponentType type) const;
	void RemoveComponent(Component * component);
	void RemoveChild(GameObject* child);

	std::string GetFileFolder() const;
	float4x4 GetGlobalTransform() const;
	void DisableBox();

	void CleanUp();
	AABB GetBoundingBox() const;

private:
	float4x4 GetLocalTransform() const;
	void ModelTransform(unsigned int shader) const;

	void DrawBBox() const;

public:
	unsigned UUID = 0;
	ComponentTransform * transform = nullptr;
	GameObject *parent = nullptr;
	std::vector<Component*> components;
	std::list<GameObject*> children;
	std::string filepath = "";
	std::string name = "GameObject";

	bool copy_flag = false;
	bool delete_flag = false;

private:
	bool drawBBox = false;

};

#endif __GameObject_h__

