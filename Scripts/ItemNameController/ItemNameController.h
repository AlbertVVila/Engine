#ifndef  __ItemNameController_h__
#define  __ItemNameController_h__

#include "BaseScript.h"

#ifdef ItemNameController_EXPORTS
#define ItemNameController_API __declspec(dllexport)
#else
#define ItemNameController_API __declspec(dllimport)
#endif
class GameObject;
class ComponentRenderer;
class ComponentText;

class ItemNameController_API ItemNameController : public Script
{
public:
	void Start() override;
	void Update() override;

	void SetNameBar(std::string name);
private:
	GameObject* itemBackground = nullptr;
	GameObject* itemTypeName = nullptr;

	ComponentRenderer* myRender = nullptr;
	ComponentText* myText = nullptr;
};

#endif __ItemNameController_h__
