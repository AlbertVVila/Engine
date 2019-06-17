#ifndef  __EquipPopupController_h__
#define  __EquipPopupController_h__

#include "BaseScript.h"
#include <vector>

#ifdef EquipPopupController_EXPORTS
#define EquipPopupController_API __declspec(dllexport)
#else
#define EquipPopupController_API __declspec(dllimport)
#endif

class GameObject;
class Text;
class ComponentImage;
class Button;

class EquipPopupController_API EquipPopupController : public Script
{

	void Start() override;
	void Update() override;

	void FillSlots();

private:
	GameObject* popupGO = nullptr;
	ComponentImage* background = nullptr;
	Text* title = nullptr;
	Button* items = nullptr;
	Button* skills = nullptr;
	Button* unequip = nullptr;
	std::vector<Button*> hudButtons;
	Button* activeButton = nullptr;
};

#endif __EquipPopupController_h__
