#ifndef __ComponentLight_h__
#define __ComponentLight_h__

#include "Component.h"
class ComponentLight :
	public Component
{
public:
	ComponentLight();
	~ComponentLight();

	ComponentLight* Clone();
};

#endif __ComponentLight_h__