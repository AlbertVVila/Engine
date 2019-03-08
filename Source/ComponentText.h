#ifndef __ComponentText_h__
#define __ComponentText_h__

#include "Component.h"
class ComponentText :
	public Component
{
public:
	ComponentText();
	ComponentText(GameObject* gameobject);
	ComponentText(const ComponentText &copy);
	~ComponentText();

	void Update() override;
	void DrawProperties() override;
	void Save(JSON_value *value) const override;
	void Load(const JSON_value &value) override;

	//variables
	float fontSize = 0.005;
	const char* text = "";
	const char* font = "";

};

#endif
