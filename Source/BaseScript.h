#ifndef  __BaseScript_h__
#define __BaseScript_h__

#include "Component.h"
#include <string>

class Application;
class GameObject;
class JSON_value;
struct ImGuiContext;

class Script : public Component
{
public:
	Script(const Script& script);
	Script(GameObject* gameobject);
	ENGINE_API Script();
	~Script();

	virtual Script* Clone() const;

	void SetApp(Application* app);
	void SetGameObject(GameObject* go);
	
	void DrawProperties();

	ENGINE_API void Load(JSON_value* json);
	ENGINE_API void Save(JSON_value* json) const;
	
	virtual void Expose(ImGuiContext* context); //TODO: Awake, OnEnable, OnDisable
	virtual void Serialize(JSON_value* json) const {} //TODO: load save
	virtual void DeSerialize(JSON_value* json) {} //TODO: Add component
	virtual void Start() {} //TODO: scene json compatibility
	virtual void Update() {}


public:
	std::string name = "Script";
protected:
	GameObject* gameObject = nullptr;
	Application* App = nullptr;

};

ENGINE_API Script* CreateScript();

#endif __BaseScript_h__