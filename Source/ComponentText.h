#ifndef __ComponentText_h__
#define __ComponentText_h__

#include "Component.h"
#include "Math/float3.h"
class ComponentText :
	public Component
{
public:
	ComponentText();
	ComponentText(GameObject* gameobject);
	ComponentText(const ComponentText &copy);
	~ComponentText();
	Component* Clone() const override;

	void DrawProperties() override;
	void Save(JSON_value *value) const override;
	void Load(const JSON_value &value) override;

	//variables
	float fontSize = 0.005;
	const char* text = "OLI";
	const char* font = "";
	float3 color = float3::zero;
};

#endif
