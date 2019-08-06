#include "EquipPopupController.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "ModuleResourceManager.h"

#include "GameObject.h"
#include "ComponentTransform2D.h"
#include "ComponentButton.h"
#include "ComponentText.h"
#include "ComponentImage.h"
#include "ResourceTexture.h"

#include "PlayerMovement.h"
#include "InventoryScript.h"
#include "SkillTreeController.h"
#include "Item.h"
#include "Skill.h"

#include "JSON.h"
#include "PlayerPrefs.h"

EquipPopupController_API Script* CreateScript()
{
	EquipPopupController* instance = new EquipPopupController;
	return instance;
}

void EquipPopupController::Start()
{
	inventory = App->scene->FindGameObjectByName("Inventory")->GetComponent<InventoryScript>();
	assert(inventory != nullptr);

	skillTree = App->scene->FindGameObjectByName("Skills")->GetComponent<SkillTreeController>();
	assert(inventory != nullptr);

	
	popupGOSkills = App->scene->FindGameObjectByName("PopUpEquipSkills", gameobject);
	popupGOItems = App->scene->FindGameObjectByName("PopUpEquipItems", gameobject);

	backgroundSkills = popupGOSkills->GetComponent<ComponentImage>();
	assert(backgroundSkills != nullptr);
	backgroundItems = popupGOItems->GetComponent<ComponentImage>();
	assert(backgroundItems != nullptr);

	items = App->scene->FindGameObjectByName("PopUpItems", gameobject)->GetComponent<Button>();
	assert(items != nullptr);

	skills = App->scene->FindGameObjectByName("PopUpSkills", gameobject)->GetComponent<Button>();
	assert(skills != nullptr);

	unequip = App->scene->FindGameObjectByName("PopUpUnequip", gameobject)->GetComponent<Button>();
	assert(unequip != nullptr);

	GameObject* HUD = App->scene->FindGameObjectByName("GameHUB");
	assert(HUD != nullptr);
	hudButtons.emplace_back(App->scene->FindGameObjectByName("RC", HUD)->GetComponent<Button>());
	hudButtons.emplace_back(App->scene->FindGameObjectByName("One", HUD)->GetComponent<Button>());
	hudButtons.emplace_back(App->scene->FindGameObjectByName("Two", HUD)->GetComponent<Button>());
	hudButtons.emplace_back(App->scene->FindGameObjectByName("Three", HUD)->GetComponent<Button>());
	hudButtons.emplace_back(App->scene->FindGameObjectByName("For", HUD)->GetComponent<Button>());
	hudButtons.emplace_back(App->scene->FindGameObjectByName("Q", HUD)->GetComponent<Button>());
	hudButtons.emplace_back(App->scene->FindGameObjectByName("W", HUD)->GetComponent<Button>());
	hudButtons.emplace_back(App->scene->FindGameObjectByName("E", HUD)->GetComponent<Button>());
	hudButtons.emplace_back(App->scene->FindGameObjectByName("R", HUD)->GetComponent<Button>());

	std::list<GameObject*> listSkills = App->scene->FindGameObjectByName("PopUpSlotsSkills", popupGOSkills)->children;
	std::list<GameObject*> listItems = App->scene->FindGameObjectByName("PopUpSlotsItems", popupGOItems)->children;

	slotsSkills = { std::begin(listSkills), std::end(listSkills) };
	slotsItems = { std::begin(listItems), std::end(listItems) };

	hoverTransform = App->scene->FindGameObjectByName("PopUpHover", gameobject)->GetComponent<Transform2D>();
	assert(hoverTransform != nullptr);

	GameObject* playerGO = App->scene->FindGameObjectByName("Player");
	assert(playerGO != nullptr);
	if (playerGO != nullptr)
	{
		player = playerGO->GetComponent<PlayerMovement>();
		assert(player != nullptr);
	}

	LoadPopUp();
}

void EquipPopupController::Update()
{
	//Check if has to close PopUp
	if (gameobject->isActive() && !backgroundSkills->isHovered && !backgroundItems->isHovered && !items->IsHovered() && !skills->IsHovered() && (App->input->GetMouseButtonDown(1) == KEY_DOWN || App->input->GetMouseButtonDown(3) == KEY_DOWN))
	{
		gameobject->SetActive(false);
		return;
	}

	//Check if has to open PopUp
	for (int i = 0; i < HUD_BUTTONS; ++i)
	{
		if (hudButtons[i]->IsHovered() && App->input->GetMouseButtonDown(3) == KEY_DOWN) {
			activeButton = i;
			if (!gameobject->isActive())
			{
				FillLists();
				gameobject->SetActive(true);
			}
			break;
		}
	}

	if (!gameobject->isActive()) { return; }

	hoverTransform->gameobject->SetActive(false);

	//Update Popup info
	ChangePopUpSlots();


	if (unequip->IsPressed())
	{
		CleanButton();
	}

	//Check if selected
	for (int i = 0; i < POPUP_SLOTS; ++i)
	{
		if (!slotsSkills[i]->isActive())
		{
			break;
		}
		if (slotsSkills[i]->GetComponent<ComponentImage>()->isHovered)
		{
			math::float2 pos = slotsSkills[i]->GetComponent<Transform2D>()->getPosition();
			hoverTransform->SetPositionUsingAligment(pos);
			hoverTransform->gameobject->SetActive(true);

			//TODO: Show info
			if (App->input->GetMouseButtonDown(1) == KEY_DOWN)
			{
				Assign(i);
			}
		}
	}

}

void EquipPopupController::Assign(int i)
{
	if (popupGOSkills->isActive())
	{
		RemoveEquiped();
		skillsEquiped.emplace_back(activeButton, skillsList[i]);
		hudButtons[activeButton]->UpdateImageByName(skillsList[i].spriteActive->GetName());
		hudButtons[activeButton]->gameobject->children.front()->GetComponent<ComponentImage>()->UpdateTexture(skillsList[i].spriteActive->GetName());
		auto it1 = std::next(hudButtons[activeButton]->gameobject->children.begin(), 1);
		(*it1)->SetActive(true);
		player->AssignSkill((SkillType)skillsList[i].id, activeButton);

	}
	else
	{
		RemoveEquiped();
		itemsEquiped.emplace_back(activeButton, itemsList[i]);
		hudButtons[activeButton]->UpdateImageByName(itemsList[i].sprite);
		hudButtons[activeButton]->gameobject->children.front()->GetComponent<ComponentImage>()->UpdateTexture(itemsList[i].sprite);
		auto it1 = std::next(hudButtons[activeButton]->gameobject->children.begin(), 1);
		(*it1)->SetActive(true);
		//TODO: Pass info to player
	}
	gameobject->SetActive(false);
}

void EquipPopupController::RemoveEquiped()
{
	for (int j = 0; j < skillsEquiped.size(); ++j)
	{
		if (skillsEquiped[j].first == activeButton)
		{
			skillsEquiped.erase(skillsEquiped.begin() + j);
		}
	}
	for (int j = 0; j < itemsEquiped.size(); ++j)
	{
		if (itemsEquiped[j].first == activeButton)
		{
			itemsEquiped.erase(itemsEquiped.begin() + j);
		}
	}
}

void EquipPopupController::ChangePopUpSlots()
{
	if (items->IsPressed() && popupGOSkills->isActive())
	{
		popupGOItems->SetActive(true);
		popupGOSkills->SetActive(false);
		FillItemSlots();
	}

	if (skills->IsPressed() && !popupGOSkills->isActive())
	{
		popupGOItems->SetActive(false);
		popupGOSkills->SetActive(true);
		FillSkillSlots();
	}
}


void EquipPopupController::FillLists()
{

	skillsList.clear();
	itemsList.clear();
	skillsList = skillTree->GetActiveSkills();
	itemsList = inventory->GetQuickItems();

	if (popupGOSkills->isActive())
	{
		FillSkillSlots();
	}
	else
	{
		FillItemSlots();
	}
}

void EquipPopupController::FillSkillSlots()
{

	for (int i = 0; i < POPUP_SLOTS; ++i)
	{
		slotsSkills[i]->SetActive(false);
		if (i < skillsList.size())
		{
			slotsSkills[i]->GetComponent<ComponentImage>()->UpdateTexture(skillsList[i].spriteActive->GetName());// = skillsList[i].spriteActive;
			slotsSkills[i]->SetActive(true);
		}
	}
}

void EquipPopupController::FillItemSlots()
{

	if (popupGOSkills->isActive()) {
		return;
	}

	for (int i = 0; i < POPUP_SLOTS; ++i)
	{
		slotsSkills[i]->SetActive(false);

		if (i < itemsList.size())
		{
			slotsSkills[i]->GetComponent<ComponentImage>()->UpdateTexture(itemsList[i].sprite);
			slotsSkills[i]->SetActive(true);
		}

	}
}

void EquipPopupController::CleanButton()
{
	auto it1 = std::next(hudButtons[activeButton]->gameobject->children.begin(), 1);
	(*it1)->SetActive(false);

	player->AssignSkill(SkillType::NONE, activeButton);

	RemoveEquiped();

	switch (activeButton)
	{
	case 0:
		hudButtons[activeButton]->UpdateImageByName("Right_clic");
		hudButtons[activeButton]->gameobject->children.front()->GetComponent<ComponentImage>()->UpdateTexture("Right_clic");
		break;
	case 1:
		hudButtons[activeButton]->UpdateImageByName("1");
		hudButtons[activeButton]->gameobject->children.front()->GetComponent<ComponentImage>()->UpdateTexture("1");
		break;
	case 2:
		hudButtons[activeButton]->UpdateImageByName("2");
		hudButtons[activeButton]->gameobject->children.front()->GetComponent<ComponentImage>()->UpdateTexture("2");
		break;
	case 3:
		hudButtons[activeButton]->UpdateImageByName("3");
		hudButtons[activeButton]->gameobject->children.front()->GetComponent<ComponentImage>()->UpdateTexture("3");
		break;
	case 4:
		hudButtons[activeButton]->UpdateImageByName("4");
		hudButtons[activeButton]->gameobject->children.front()->GetComponent<ComponentImage>()->UpdateTexture("4");
		break;
	case 5:
		hudButtons[activeButton]->UpdateImageByName("Q");
		hudButtons[activeButton]->gameobject->children.front()->GetComponent<ComponentImage>()->UpdateTexture("Q");
		break;
	case 6:
		hudButtons[activeButton]->UpdateImageByName("W");
		hudButtons[activeButton]->gameobject->children.front()->GetComponent<ComponentImage>()->UpdateTexture("W");
		break;
	case 7:
		hudButtons[activeButton]->UpdateImageByName("E");
		hudButtons[activeButton]->gameobject->children.front()->GetComponent<ComponentImage>()->UpdateTexture("E");
		break;
	case 8:
		hudButtons[activeButton]->UpdateImageByName("R");
		hudButtons[activeButton]->gameobject->children.front()->GetComponent<ComponentImage>()->UpdateTexture("R");
		break;
	default:
		break;
	}
}

void EquipPopupController::SavePopUp()
{
	JSON* popup = new JSON();
	JSON_value *skillsJSON = popup->CreateValue(rapidjson::kArrayType);
	for (int i = 0; i < skillsEquiped.size(); ++i)
	{
		JSON_value *skill = skillsJSON->CreateValue();
		skill->AddInt("id", (int)skillsEquiped[i].second.id);
		skill->AddString("name", skillsEquiped[i].second.name.c_str());
		skill->AddString("description", skillsEquiped[i].second.description.c_str());
		skill->AddString("sprite", skillsEquiped[i].second.spriteActive->GetName());
		skill->AddInt("currentLevel", skillsEquiped[i].second.currentLevel);
		skill->AddInt("position", skillsEquiped[i].first);
		skillsJSON->AddValue("", *skill);
	}
	popup->AddValue("skills", *skillsJSON);

	JSON_value *itemsJSON = popup->CreateValue(rapidjson::kArrayType);
	for (int i = 0; i < itemsEquiped.size(); ++i)
	{
		JSON_value *item = itemsJSON->CreateValue();
		item->AddInt("type", (int)itemsEquiped[i].second.type);
		item->AddString("name", itemsEquiped[i].second.name.c_str());
		item->AddString("description", itemsEquiped[i].second.description.c_str());
		item->AddString("sprite", itemsEquiped[i].second.sprite.c_str());
		item->AddInt("equiped", itemsEquiped[i].second.isEquipped);
		item->AddFloat("dexterity", itemsEquiped[i].second.stats.dexterity);
		item->AddFloat("health", itemsEquiped[i].second.stats.health);
		item->AddFloat("hpRegen", itemsEquiped[i].second.stats.hpRegen);
		item->AddFloat("mana", itemsEquiped[i].second.stats.mana);
		item->AddFloat("manaRegen", itemsEquiped[i].second.stats.manaRegen);
		item->AddFloat("strength", itemsEquiped[i].second.stats.strength);
		item->AddInt("position", itemsEquiped[i].first);
		itemsJSON->AddValue("", *item);
	}
	popup->AddValue("items", *itemsJSON);
	PlayerPrefs::SaveJson(popup, "PopUp");
}

void EquipPopupController::LoadPopUp()
{
	JSON* popup = PlayerPrefs::LoadJson("PopUp");
	if (popup != nullptr) {
		JSON_value* itemsJSON = popup->GetValue("items");
		itemsEquiped.clear();
		for (unsigned i = 0; i < itemsJSON->Size(); i++)
		{
			JSON_value* itemJSON = itemsJSON->GetValue(i);
			int position = itemJSON->GetInt("position");
			Item item;
			item.type = (ItemType)itemJSON->GetInt("type");
			item.name = itemJSON->GetString("name");
			item.description = itemJSON->GetString("description");
			item.sprite = itemJSON->GetString("sprite");
			item.isEquipped = itemJSON->GetInt("equiped");
			item.stats.dexterity = itemJSON->GetFloat("dexterity");
			item.stats.health = itemJSON->GetFloat("health");
			item.stats.hpRegen = itemJSON->GetFloat("hpRegen");
			item.stats.mana = itemJSON->GetFloat("mana");
			item.stats.manaRegen = itemJSON->GetFloat("manaRegen");
			item.stats.strength = itemJSON->GetFloat("strength");

			itemsEquiped.emplace_back(position, item);
			hudButtons[position]->UpdateImageByName(item.sprite);
			hudButtons[position]->gameobject->children.front()->GetComponent<ComponentImage>()->UpdateTexture(item.sprite);
			auto it1 = std::next(hudButtons[position]->gameobject->children.begin(), 1);
			(*it1)->SetActive(true);
		}

		JSON_value* skillsJSON = popup->GetValue("skills");
		skillsEquiped.clear();
		for (unsigned i = 0; i < skillsJSON->Size(); i++)
		{
			JSON_value* skillJSON = skillsJSON->GetValue(i);
			int position = skillJSON->GetInt("position");
			Skill skill;
			skill.id = skillJSON->GetInt("id");
			skill.name = skillJSON->GetString("name");
			skill.description = skillJSON->GetString("description");
			skill.spriteActive = (ResourceTexture*)App->resManager->GetByName(skillJSON->GetString("sprite"), TYPE::TEXTURE);
			skill.currentLevel = skillJSON->GetInt("currentLevel");

			skillsEquiped.emplace_back(position, skill);
			hudButtons[position]->UpdateImageByName(skill.spriteActive->GetName());
			hudButtons[position]->gameobject->children.front()->GetComponent<ComponentImage>()->UpdateTexture(skill.spriteActive->GetName());
			auto it1 = std::next(hudButtons[position]->gameobject->children.begin(), 1);
			(*it1)->SetActive(true);
		}

	}
}
