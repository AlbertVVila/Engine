#ifndef __Component_h__
#define __Component_h__

class Component
{
public:
	Component();
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
	bool enabled = true;
};

#endif __Component_h__