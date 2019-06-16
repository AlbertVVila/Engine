#ifndef  __EnemyLoot_h__
#define  __EnemyLoot_h__

#include "BaseScript.h"

#ifdef EnemyLoot_EXPORTS
#define EnemyLoot_API __declspec(dllexport)
#else
#define EnemyLoot_API __declspec(dllimport)
#endif

#include <vector>
#include "Math/float2.h"
#include "Item.h"
#include "imgui.h"

class GameObject;
class Script;
class ItemPicker;

class EnemyLoot_API EnemyLoot : public Script
{
public:
	void Start() override;
	void Update() override;
	void GenerateLoot();
	void AddItem(std::string name, int drop);

private:

	std::vector<std::pair<GameObject*, int>> items;
	int drop = 0;
	std::string goName = "GO Name";
	std::string itemName = "";

	GameObject* go = nullptr;

	Script* script = nullptr;

	ItemPicker* itemPicker;

	void Expose(ImGuiContext * context);
};

#endif __EnemyLoot_h__
