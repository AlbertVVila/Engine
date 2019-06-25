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
class Item;
class Skill;
class SkillTreeController;
class InventoryScript;
class Transform2D;

class EquipPopupController_API EquipPopupController : public Script
{

	void Start() override;
	void Update() override;

public:
	void SavePopUp();


private:

	void Assign(int i);
	void RemoveEquiped();
	void ChangePopUpSlots();
	void FillLists();
	void FillSkillSlots();
	void FillItemSlots();
	void CleanButton();
	void LoadPopUp();


	InventoryScript* inventory = nullptr;
	SkillTreeController* skillTree = nullptr;

	GameObject* popupGO = nullptr;
	ComponentImage* background = nullptr;
	Text* title = nullptr;
	Button* items = nullptr;
	Button* skills = nullptr;
	Button* unequip = nullptr;
	std::vector<GameObject*> slots;
	Transform2D* hoverTransform = nullptr;

	std::vector<Button*> hudButtons;
	int activeButton = -1;

	std::vector<Item> itemsList;
	std::vector<Skill> skillsList;

	std::vector<std::pair<int, Skill>> skillsEquiped;
	std::vector<std::pair<int, Item>> itemsEquiped;


	bool skillsShowing = true;
};

#endif __EquipPopupController_h__
