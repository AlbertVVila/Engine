#ifndef  __BaseScript_h__
#define __BaseScript_h__

class Application;
class GameObject;
class JSON;
struct ImGuiContext;

#ifdef BASESCRIPT_EXPORTS
#define  BASESCRIPT_API __declspec(dllexport)
#else
#define BASESCRIPT_API __declspec(dllimport)
#endif

class BASESCRIPT_API Script
{
public:
	void SetApp(Application * app);
	void SetGameObject(GameObject* go);
	virtual void Expose(ImGuiContext *context);
	//virtual void Serialize(JSON* json) const = 0;
	//virtual void DeSerialize(JSON * json) = 0;
	virtual void Start() {}
	virtual void Update() {}

protected:
	GameObject* gameObject;
	Application* App;

};

#endif __BaseScript_h__