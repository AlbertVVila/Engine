#ifndef __GameObject_h__
#define __GameObject_h__

#include <vector>
#include <list>
#include "Math/float4x4.h"

class Component;
class ComponentTransform;
class Frustum;
enum class ComponentType;
struct Texture;
struct JSON_value;

class GameObject
{
public:
	GameObject() {};
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

	float4x4 GetGlobalTransform() const;
	void DisableBox();

	bool CleanUp();
	void Save(JSON_value *gameobjects) const;
	void Load(const JSON_value & gameobject);
	AABB GetBoundingBox() const;

private:
	float4x4 GetLocalTransform() const;
	void UpdateModel(unsigned int shader) const;

	void DrawBBox() const; //TODO: improve BBOX draw

public:
	unsigned UUID = 0;
	unsigned parentUUID = 0; //only set in Save/Load scene TODO:update on parent change
	ComponentTransform * transform = nullptr;
	GameObject *parent = nullptr;
	std::vector<Component*> components;
	std::list<GameObject*> children;
	std::string name = "GameObject";

	bool is_static = false;
	bool copy_flag = false;
	bool delete_flag = false;

private:
	bool drawBBox = false;

};

#endif __GameObject_h__

