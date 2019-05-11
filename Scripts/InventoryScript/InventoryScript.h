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

class Component;
class GameObject;
class Transform2D;

#define TOTAL_SLOTS 25
#define INVENTARY_SLOTS 18

class InventoryScript_API InventoryScript : public Script
{
public:
	void Start() override;
	void Update() override;

private:
	std::vector<Component*> slotsTransform;

	std::vector<GameObject*> items;
	GameObject* inventory = nullptr;

	math::float2 initialitemPos = math::float2::zero;
};

#endif __InventoryScript_h__
