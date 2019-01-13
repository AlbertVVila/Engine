#ifndef __GameObject_h__
#define __GameObject_h__

#include <vector>
#include <list>
#include "Geometry/AABB.h"

class Component;
class ComponentTransform;
enum class ComponentType;
struct Texture;
struct JSON_value;

class GameObject
{
public:
	GameObject() {};
	GameObject(const char* name, unsigned uuid);
	GameObject(const float4x4& transform, const char* name, unsigned uuid);
	GameObject(const GameObject& gameobject);
	~GameObject();

	void DrawProperties();
	void DrawHierarchy(GameObject * obj_clicked);

	void Update();

	Component * CreateComponent(ComponentType type);
	Component * GetComponent(ComponentType type) const;
	std::vector<Component *> GetComponents(ComponentType type) const;
	std::vector<Component *> GetComponentsInChildren(ComponentType type) const;
	void RemoveComponent(Component * component);
	void RemoveChild(GameObject* child);

	void UpdateGlobalTransform();
	void SetGlobalTransform(const float4x4 &global);
	float4x4 GetGlobalTransform() const;
	float4x4 GetLocalTransform() const;

	void UpdateBBox();
	void DrawBBox() const; //TODO: improve BBOX draw
	AABB GetBoundingBox() const;
	bool Intersects(const LineSegment & line, float* distance) const;
	void UpdateModel(unsigned int shader) const;
	void SetLightUniforms(unsigned shader) const;

	bool CleanUp();
	void Save(JSON_value *gameobjects) const;
	void Load(JSON_value * gameobject);

private:
	bool IsParented(const GameObject & gameobject);
	void SetStaticAncestors();

public:
	unsigned UUID = 0;
	unsigned parentUUID = 0; //only set in Save/Load scene TODO:update on parent change
	bool isStatic = false;
	bool moved_flag = false;
	bool copy_flag = false;
	bool delete_flag = false;
	bool drawBBox = false;
	AABB bbox;

	ComponentTransform * transform = nullptr;
	GameObject *parent = nullptr;

	std::vector<Component*> components;
	std::list<GameObject*> children;

	std::string name = "GameObject";

};

#endif __GameObject_h__

