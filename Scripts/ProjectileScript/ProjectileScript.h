#ifndef  __ProjectileScript_h__
#define  __ProjectileScript_h__

#ifdef ProjectileScript_EXPORTS
#define ProjectileScript_API __declspec(dllexport)
#else
#define ProjectileScript_API __declspec(dllimport)
#endif

#include "BaseScript.h"

class JSON_value;
class ComponentTransform;

class ProjectileScript_API ProjectileScript : public Script
{

	void Start() override;
	void Update() override;

	void Expose(ImGuiContext* context) override;

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

public:
	float speed = 10.0f;
	float lifeTime = 10.0f;

private:
	ComponentTransform* transform;
};

#endif __ProjectileScript_h__
