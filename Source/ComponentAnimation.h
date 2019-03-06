#ifndef __COMPONENTANIMATION_H_
#define __COMPONENTANIMATION_H_

#include "Component.h"
#include "Animation.h"

class ComponentAnimation : public Component
{
public:

	void DrawProperties();

	Component* Clone() const;

	ComponentAnimation();
	ComponentAnimation(GameObject* gameobject);
	ComponentAnimation(const ComponentAnimation &copy);
	~ComponentAnimation();

	void Save(JSON_value* value) const override;
	void Load(JSON_value* value) override;

	bool CleanUp();
public:

	Animation* anim = nullptr;
};

#endif //  __COMPONENTANIMATION_H_
