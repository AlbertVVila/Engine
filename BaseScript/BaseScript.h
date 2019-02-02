#ifndef  __BaseScript_h__
#define __BaseScript_h__

class GameObject;
struct ImGuiContext;

#ifdef BASESCRIPT_EXPORTS
#define BASESCRIPT_API __declspec(dllexport)

#else
#define BASESCRIPT_API __declspec(dllimport)
#endif

class BASESCRIPT_API Script
{
public:
	void SetGameObject(GameObject* go);
	void Expose(ImGuiContext *context);

	virtual void Start();
	virtual void Update();

protected:
	GameObject * gameObject;

};

#endif __BaseScript_h__