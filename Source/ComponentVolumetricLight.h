#ifndef __ComponentVolumetricLight_h__
#define __ComponentVolumetricLight_h__

#include "Component.h"
#include "Math/float4.h"
#include <vector>

class ResourceTexture;

class ComponentVolumetricLight : public Component
{
public:
	ComponentVolumetricLight();
	ComponentVolumetricLight(const ComponentVolumetricLight &copy);

	Component* Clone() const override;

	void Update() override;

	void DrawProperties() override;
	void Save(JSON_value* value) const override;
	void Load(JSON_value* value) override;

};

#endif // __ComponentVolumetricLight_h__

