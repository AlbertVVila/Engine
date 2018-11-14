#ifndef __Component_h__
#define __Component_h__

enum ComponentType
{
	Transform,
	Mesh,
	Material,
	Light
};

class Component
{
public:
	Component(ComponentType type);
	virtual ~Component();

	virtual void Enable()
	{
		enabled = true;
	}

	virtual void Update() {}
	virtual void Disable()
	{
		enabled = false;
	}

public:
	ComponentType type;
	bool enabled = true;
};

#endif __Component_h__