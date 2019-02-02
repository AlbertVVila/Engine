#ifndef __ComponentScript_h__
#define __ComponentScript_h__

#include "Component.h"
#include <string>

class GameObject;
class Script;

class ComponentScript :
	public Component
{

public:
	ComponentScript(GameObject* gameobject);

	ComponentScript(const ComponentScript & component);

	~ComponentScript();

	ComponentScript * Clone() const;

	void Update() override;
	void DrawProperties();

private:
	std::string scriptName = "No Script";
	Script* script;
};

#endif
