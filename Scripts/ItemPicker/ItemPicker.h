#ifndef  __ItemPicker_h__
#define  __ItemPicker_h__

#include "BaseScript.h"

#include "Item.h"

#ifdef ItemPicker_EXPORTS
#define ItemPicker_API __declspec(dllexport)
#else
#define ItemPicker_API __declspec(dllimport)
#endif

class ItemPicker_API ItemPicker : public Script
{
	void Expose(ImGuiContext* context) override;

	void Start() override;
	void Update() override;

private:
	std::string name;
	std::string description;
	std::string sprite;
	ItemType type = ItemType::NONE;

	Item item;


};

#endif __ItemPicker_h__
