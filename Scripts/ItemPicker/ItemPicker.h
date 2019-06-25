#ifndef  __ItemPicker_h__
#define  __ItemPicker_h__

#include "BaseScript.h"
#include "Item.h"
#include "Geometry/AABB.h"
#include <vector>
#include <list>

#ifdef ItemPicker_EXPORTS
#define ItemPicker_API __declspec(dllexport)
#else
#define ItemPicker_API __declspec(dllimport)
#endif

class InventoryScript;
class AABB;
class JSON_value;
class ComponentAudioSource;
class ComponentRenderer;
class ItemNameController;

enum class ItemPicker_API ItemRarity
{
	BASIC = 0,
	RARE,
	EPIC,
	LEGENDARY
};

class ItemPicker_API ItemPicker : public Script
{
public:
	void Expose(ImGuiContext* context) override;

	void Start() override;
	void Update() override;

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;
	void SetItem(ItemType type, std::string name, std::string sprite);

	std::string name;
	std::string description;
	std::string sprite;
	ItemType type = ItemType::NONE;
	Item item;

private:

	InventoryScript* inventoryScript = nullptr;
	math::AABB* playerBbox = nullptr;

	std::vector<std::string> textureFiles;
	const char * selectedTexture = "basicPostion";

	ComponentAudioSource* itemPickedAudio = nullptr;
	std::string myBboxName;

	ComponentRenderer* myRender;
	ItemRarity rarity = ItemRarity::BASIC;
	int rare = 0;

	ItemNameController* itemName = nullptr;
	std::list<unsigned> nameShowed;

	float time = -1.0f;
};

#endif __ItemPicker_h__
