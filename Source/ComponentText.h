#ifndef __ComponentText_h__
#define __ComponentText_h__

#include "Component.h"

#include "Math/float4.h"

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
public:
	float fontSize = 14;
	std::string text = "Fractal Text";
	std::string font = "";
	float4 color = float4(1.0f, 1.0f, 1.0f, 0.0f);
};

#endif
