#ifndef __Component_h__
#define __Component_h__

class GameObject;

enum ComponentType //TODO: enum class
{
	Transform = 0,
	Mesh,
	Material,
	Light
};

class Component
{
public:
	Component(GameObject* gameobject, ComponentType type);
	virtual ~Component();

	bool DrawComponentState();

	virtual void DrawProperties() {};
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

public:
	GameObject* gameobject = nullptr;
	ComponentType type;
	bool enabled = true;
};

#endif __Component_h__