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
	ComponentScript(const ComponentScript& component);
	~ComponentScript();

	void DrawProperties();
	void Save(JSON_value* value) const override;
	void Load(JSON_value* value) override;

	void SetScript(const std::string& name);

	inline Script* GetScript() const
	{
		return script;
	}

	inline std::string GetScriptName() const
	{
		return scriptName;
	}

	void ScriptStart() const;
	void ScriptUpdate() const;

	ComponentScript* Clone() const;

private:
	std::string scriptName = "No Script";
	Script* script = nullptr;
};

#endif
