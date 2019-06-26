#ifndef  __InventoryScript_h__
#define  __InventoryScript_h__

#include "BaseScript.h"

#ifdef InventoryScript_EXPORTS
#define InventoryScript_API __declspec(dllexport)
#else
#define InventoryScript_API __declspec(dllimport)
#endif

#include <vector>
#include "Math/float2.h"
#include "Item.h"

class Component;
class GameObject;
class Transform2D;
class ComponentAudioSource;
class PlayerMovement;

#define TOTAL_SLOTS 24
#define INVENTARY_SLOTS 18

class InventoryScript_API InventoryScript : public Script
{
public:
	void Awake() override;
	void Start() override;
	void Update() override;

	inline virtual InventoryScript* Clone() const
	{
		return new InventoryScript(*this);
	}

	bool AddItem(Item item);
	std::vector<Item> GetQuickItems();

private:

	std::vector<Component*> slotsTransform;
	std::vector<GameObject*> itemsSlots;
	std::vector<std::pair<Item, int>> items;

	GameObject* inventory = nullptr;
	Transform2D* menuPlayer = nullptr;

	math::float2 initialitemPos = math::float2::zero;

	ComponentAudioSource* selectItemAudio;
	ComponentAudioSource* dropItemAudio;

	PlayerMovement* playerMovement = nullptr;

	bool itemGrabbed = false; 
};

#endif __InventoryScript_h__
