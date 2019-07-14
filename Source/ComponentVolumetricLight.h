#ifndef __ComponentVolumetricLight_h__
#define __ComponentVolumetricLight_h__

#include "Component.h"
#include "Math/float4.h"
#include <vector>

class GameObject;

class ComponentVolumetricLight : public Component
{
public:
	ComponentVolumetricLight(GameObject* gameobject);
	ComponentVolumetricLight(const ComponentVolumetricLight &copy);
	
	Component* Clone() const override;

	void Update() override;

	void DrawProperties() override;
	void Save(JSON_value* value) const override;
	void Load(JSON_value* value) override;

	void Init();

private:

	float conePoints[192];
	float coneUVs[128];
	int coneIndexes[65];
	float circle1Radius = 1.0f;
	float circle2Radius = 5.0f;
	float lenght = 1.0f;
};

#endif // __ComponentVolumetricLight_h__

