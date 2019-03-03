#ifndef __COMPONENTBONE_H_
#define __COMPONENTBONE_H_

#include "Component.h"

class ComponentBone : public Component
{
public:
	ComponentBone();
	ComponentBone(GameObject* gameobject);
	ComponentBone(const ComponentBone &copy);
	~ComponentBone();

	ComponentBone* Clone() const;


	void DrawProperties() override;
	void DrawDebug();

	void Save(JSON_value *value) const override;
	void Load(const JSON_value &value) override;

	unsigned UID = 0u;
};

#endif // __COMPONENTBONE_H_