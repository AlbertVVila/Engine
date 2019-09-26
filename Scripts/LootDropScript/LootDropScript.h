#ifndef  __LootDropScript_h__
#define  __LootDropScript_h__

#include "BaseScript.h"
#include <vector>

#include "Item.h"
#include "Math\float3.h"

class ImGuiContext;

#ifdef LootDropScript_EXPORTS
#define LootDropScript_API __declspec(dllexport)
#else
#define LootDropScript_API __declspec(dllimport)
#endif

class LootDropScript_API LootDropScript : public Script
{
	void Start() override;
	void Expose(ImGuiContext* context);

public:
	void DropItems();

	std::string itemPrefabName = "";
	//int numberOfItems = 1;
	//std::vector<std::string> itemList;

private:
	GameObject* itemListGO = nullptr;
};

#endif __LootDropScript_h__
