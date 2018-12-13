#ifndef __Component_h__
#define __Component_h__

class GameObject;
struct JSON_value;

enum ComponentType //TODO: enum class
{
	Transform = 0,
	Mesh,
	Material,
	Camera,
	Light
};

class Component
{
public:
	Component(GameObject* gameobject, ComponentType type);
	Component(const Component& component);

	virtual ~Component();
	virtual Component* Clone() = 0;

	bool DrawComponentState();

	virtual void DrawProperties() = 0;
	virtual void Enable()
	{
		enabled = true;
	}

	virtual void Update() {}
	virtual void Disable()
	{
		enabled = false;
	}
	
	virtual void CleanUp() {}

	void Remove();

	virtual void Save(JSON_value *value) const;
	virtual void Load(JSON_value *value);

public:
	GameObject* gameobject = nullptr;
	ComponentType type;
	bool enabled = true;
};

#endif __Component_h__