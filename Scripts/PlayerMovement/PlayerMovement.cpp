#include "PlayerMovement.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleTime.h"
#include "ModuleScene.h"
#include "ModuleNavigation.h"
#include "ModuleUI.h"
#include "CameraController.h"
#include "PlayerState.h"
#include "PlayerStateAttack.h"
#include "PlayerStateIdle.h"
#include "PlayerStateWalk.h"
#include "PlayerStateDeath.h"
#include "EnemyControllerScript.h"

#include "ComponentAnimation.h"
#include "ComponentBoxTrigger.h"
#include "ComponentTransform.h"
#include "ComponentImage.h"
#include "ComponentRenderer.h"
#include "ComponentCamera.h"
#include "ComponentText.h"
#include "GameObject.h"

#include "DamageController.h"
#include "DamageFeedbackUI.h"
#include "EnemyControllerScript.h"

#include "BasicSkill.h"
#include "SliceSkill.h"
#include "ChainAttackSkill.h"
#include "DashSkill.h"
#include "BombDropSkill.h"
#include "CircularAttackSkill.h"
#include "StompSkill.h"
#include "RainSkill.h"

#include "JSON.h"
#include <assert.h>
#include <string>
#include "imgui.h"
#include "Globals.h"
#include "debugdraw.h"

#include "ComponentAudioSource.h"
#include "PlayerPrefs.h"

#define SKILLS_SLOTS 5

PlayerMovement_API Script* CreateScript()
{
	PlayerMovement* instance = new PlayerMovement;
	return instance;
}

PlayerMovement::PlayerMovement()
{
	// Register Skills
	allSkills[SkillType::NONE] = new PlayerSkill();
	allSkills[SkillType::STOMP] = new PlayerSkill(SkillType::STOMP);
	allSkills[SkillType::RAIN] = new PlayerSkill(SkillType::RAIN);
	allSkills[SkillType::CHAIN] = new PlayerSkill(SkillType::CHAIN);
	allSkills[SkillType::DASH] = new PlayerSkill(SkillType::DASH);
	allSkills[SkillType::SLICE] = new PlayerSkill(SkillType::SLICE);
	allSkills[SkillType::BOMB_DROP] = new PlayerSkill(SkillType::BOMB_DROP);
	allSkills[SkillType::CIRCULAR] = new PlayerSkill(SkillType::CIRCULAR);

	// Default ability keyboard allocation
	assignedSkills[HUB_BUTTON_RC] = SkillType::CHAIN;
	assignedSkills[HUB_BUTTON_1] = SkillType::STOMP;
	assignedSkills[HUB_BUTTON_2] = SkillType::RAIN;
	assignedSkills[HUB_BUTTON_3] = SkillType::NONE;
	assignedSkills[HUB_BUTTON_4] = SkillType::NONE;
	assignedSkills[HUB_BUTTON_Q] = SkillType::DASH;
	assignedSkills[HUB_BUTTON_W] = SkillType::SLICE;
	assignedSkills[HUB_BUTTON_E] = SkillType::BOMB_DROP;
	assignedSkills[HUB_BUTTON_R] = SkillType::CIRCULAR;
}

PlayerMovement::~PlayerMovement()
{
	for (auto it = allSkills.begin(); it != allSkills.end(); ++it) delete it->second;
	allSkills.clear();
}

void PlayerMovement::Expose(ImGuiContext* context)
{
	ImGui::Text("Cooldowns");
	if (ImGui::Checkbox("Show Ability Cooldown", &showAbilityCooldowns)) ActivateHudCooldownMask(showAbilityCooldowns, HUB_BUTTON_RC, HUB_BUTTON_R);
	ImGui::DragFloat("General Ability Cooldown", &hubGeneralAbilityCooldown, 1.0F, 0.0F, 10.0F);
	ImGui::DragFloat("RC Cooldown", &hubCooldown[HUB_BUTTON_RC], 1.0F, 0.0F, 10.0F);
	ImGui::DragFloat("1 Cooldown", &hubCooldown[HUB_BUTTON_1], 1.0F, 0.0F, 10.0F);
	ImGui::DragFloat("2 Cooldown", &hubCooldown[HUB_BUTTON_2], 1.0F, 0.0F, 10.0F);
	ImGui::DragFloat("3 Cooldown", &hubCooldown[HUB_BUTTON_3], 1.0F, 0.0F, 10.0F);
	ImGui::DragFloat("4 Cooldown", &hubCooldown[HUB_BUTTON_4], 1.0F, 0.0F, 10.0F);
	ImGui::DragFloat("Q Cooldown", &hubCooldown[HUB_BUTTON_Q], 1.0F, 0.0F, 10.0F);
	ImGui::DragFloat("W Cooldown", &hubCooldown[HUB_BUTTON_W], 1.0F, 0.0F, 10.0F);
	ImGui::DragFloat("E Cooldown", &hubCooldown[HUB_BUTTON_E], 1.0F, 0.0F, 10.0F);
	ImGui::DragFloat("R Cooldown", &hubCooldown[HUB_BUTTON_R], 1.0F, 0.0F, 10.0F);

	/*if (ImGui::Checkbox("Show Item Cooldown", &showItemCooldowns)) ActivateHudCooldownMask(showItemCooldowns, HUB_BUTTON_1, HUB_BUTTON_4);
	ImGui::DragFloat("1 Cooldown", &hubCooldown[HUB_BUTTON_1], 1.0F, 0.0F, 10.0F);*/

	ImGui::DragFloat("Bomb drop grow rate", &bombDropGrowRate, 0.01f, 1.0f, 10.0f);
	ImGui::SetCurrentContext(context);

	//Exposing durations this should access to every class instead of allocating them in PlayerMovement, but for now scripts don't allow it
	ImGui::DragFloat("Walking speed", &walkingSpeed, 0.01f, 10.f, 500.0f);
	ImGui::DragFloat("Out of NavMesh pos correction XZ", &OutOfMeshCorrectionXZ, 1.f, 0.f, 1000.f);
	ImGui::DragFloat("Out of NavMesh pos correction Y", &OutOfMeshCorrectionY, 1.f, 0.f, 500.f);
	ImGui::DragFloat("Max walking distance", &maxWalkingDistance, 100.f, 0.f, 100000.f);
	ImGui::DragFloat("Stop distance", &straightPathingDistance, 100.f, 500.f, 10000.f);

	ImGui::DragFloat("Out of Combat time", &outCombatMaxTime, 1.f, 0.f, 10.f);
	
	float maxHP = stats.health;
	float maxMP = stats.mana;
	stats.Expose("Player Stats");
	if (maxHP != stats.health)
	{
		health += stats.health - maxHP;
		if (health > stats.health) health = stats.health;
		else if (health < 0) health = 0;
	}
	if (maxMP != stats.mana)
	{
		mana += stats.mana - maxMP;
		if (mana > stats.mana) mana = stats.mana;
		else if (mana < 0) mana = 0;
	}

	UpdateUIStats();

	ImGui::Spacing();
	ImGui::Text("HP/MP Regen Timers");
	ImGui::DragFloat("Out of Combat time", &outCombatMaxTime, 1.f, 0.f, 10.f);
	ImGui::DragFloat("After skill time (MP)", &manaRegenMaxTime, 1.f, 0.f, 10.f);

	ImGui::Spacing();
	ImGui::Text("Cooldowns");
	if (ImGui::Checkbox("Show Ability Cooldown", &showAbilityCooldowns)) ActivateHudCooldownMask(showAbilityCooldowns, HUB_BUTTON_RC, HUB_BUTTON_R);
	ImGui::DragFloat("General Ability Cooldown", &hubGeneralAbilityCooldown, 1.0F, 0.0F, 10.0F);

	ImGui::Spacing();
	ImGui::Text("Skills");

	ImGui::Separator();
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Current skill: "); ImGui::SameLine();

	if (currentSkill == chain)			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Chain");
	else if (currentSkill == dash)		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Dash");
	else if (currentSkill == slice)		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Slice");
	else if (currentSkill == bombDrop)	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Bomb Drop");
	else if (currentSkill == circular)	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Circular");
	else if (currentSkill == stomp)		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Stomp");
	else if (currentSkill == rain)		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Rain");
	else 								ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "None");

	for (auto it = allSkills.begin(); it != allSkills.end(); ++it)
	{
		switch (it->first)
		{
		case SkillType::CHAIN: it->second->Expose("Chain Attack"); break;
		case SkillType::DASH: it->second->Expose("Dash"); break;
		case SkillType::SLICE: it->second->Expose("Slice"); break;
		case SkillType::BOMB_DROP: it->second->Expose("Bomb Drop"); break;
		case SkillType::CIRCULAR: it->second->Expose("Circular Attack"); break;
		case SkillType::STOMP: it->second->Expose("Stomp Attack"); break;
		case SkillType::RAIN: it->second->Expose("Rain"); break;
		case SkillType::NONE:
		default:
			break;
		}
	}

	ImGui::Spacing();

	// Stats Debug
	ImGui::Text("Play Stats Debug");
	ImGui::Text("HP: %f / %f", health, stats.health);
	ImGui::Text("MP: %f / %f", mana, stats.mana);
	ImGui::Text("Strength: %i", stats.strength);
	ImGui::Text("Dexterity: %i", stats.dexterity);
	ImGui::Text("HP Regen: %f pts/s", stats.hpRegen);
	ImGui::Text("MP Regen: %f pts/s", stats.manaRegen);
}

void PlayerMovement::CreatePlayerStates()
{
	playerStates.reserve(5);

	playerStates.push_back(walk = new PlayerStateWalk(this, "Walk"));
	if (dustParticles == nullptr)
	{
		LOG("Dust Particles not found");
	}
	else
	{
		LOG("Dust Particles found");
		dustParticles->SetActive(false);
		walk->dustParticles = dustParticles;
	}
	playerStates.push_back(attack = new PlayerStateAttack(this, "Attack",
		math::float3(150.f, 100.f, 100.f), 0.7f, 0.9f));
	playerStates.push_back(idle = new PlayerStateIdle(this, "Idle"));
	playerStates.push_back(death = new PlayerStateDeath(this, "Death"));
}

void PlayerMovement::CreatePlayerSkills()
{
	chain = new ChainAttackSkill(this, "Chain1", attackBoxTrigger);
	dash = new DashSkill(this, "Dash", attackBoxTrigger);
	slice = new SliceSkill(this, "Slice", attackBoxTrigger);
	bombDrop = new BombDropSkill(this, "BombDrop", attackBoxTrigger);
	bombDropParticles = App->scene->FindGameObjectByName("BombDropParticles", gameobject);
	circular = new CircularAttackSkill(this, "Circular", attackBoxTrigger);
	circular->mesh1 = App->scene->FindGameObjectByName("CircularAttackMesh1");
	circular->mesh2 = App->scene->FindGameObjectByName("CircularAttackMesh2");
	circular->mesh3 = App->scene->FindGameObjectByName("Spiral");
	circular->particles = App->scene->FindGameObjectByName("CircularAttackParticles");
	stomp = new StompSkill(this, "Stomp", attackBoxTrigger);
	rain = new RainSkill(this, "Rain", "RainPrefab");
	rain->machetes = App->scene->FindGameObjectByName("MacheteRain");
	if (rain->machetes)
	{
		macheteRainRenderer = rain->machetes->GetComponent<ComponentRenderer>();
		if (macheteRainRenderer)
		{
			macheteRainRenderer->dissolve = true;
			macheteRainRenderer->borderAmount = 0.04f;
		}
	}
	else
	{
		LOG("Machete rain mesh not found");
	}
	macheteRainParticles = App->scene->FindGameObjectByName("MacheteRainParticles");
	if (!macheteRainParticles)
	{
		LOG("Machete rain particles not found");
	}

	playerCamera = App->scene->FindGameObjectByName("PlayerCamera");
	if (!playerCamera)
	{
		LOG("Player camera not found");
	}
	allSkills[SkillType::CHAIN]->skill = (BasicSkill*)chain;
	allSkills[SkillType::DASH]->skill = (BasicSkill*)dash;
	allSkills[SkillType::SLICE]->skill = (BasicSkill*)slice;
	allSkills[SkillType::BOMB_DROP]->skill = (BasicSkill*)bombDrop;
	allSkills[SkillType::CIRCULAR]->skill = (BasicSkill*)circular;
	allSkills[SkillType::STOMP]->skill = (BasicSkill*)stomp;
	allSkills[SkillType::RAIN]->skill = (BasicSkill*)rain;
}

void PlayerMovement::Start()
{
	dustParticles = App->scene->FindGameObjectByName("WalkingDust");
	dashFX = App->scene->FindGameObjectByName("DashFX");
	dashMesh = App->scene->FindGameObjectByName("DashMesh");

	GameObject* damageGO = App->scene->FindGameObjectByName("Damage");
	if (damageGO == nullptr)
	{
		LOG("Damage controller GO couldn't be found \n");
	}
	else
	{
		damageController = damageGO->GetComponent<DamageController>();
		if (damageController == nullptr)
		{
			LOG("Damage controller couldn't be found \n");
		}
	}

	GameObject* damageFeedback = App->scene->FindGameObjectByName("RedScreen");
	if (damageFeedback == nullptr)
	{
		LOG("Damage Feedback blood couldn't be found \n");
	}
	else
	{
		damageUIFeedback = damageFeedback->GetComponent<DamageFeedbackUI>();
		if (damageUIFeedback == nullptr)
		{
			LOG("Damage UI feedback script couldn't be found \n");
		}
	}
	
	CreatePlayerStates();

	currentState = idle;

	anim = gameobject->GetComponent<ComponentAnimation>();
	if (anim == nullptr)
	{
		LOG("The GameObject %s has no Animation component attached \n", gameobject->name);
	}

	GameObject* hitBoxAttackGameObject = App->scene->FindGameObjectByName("HitBoxAttack", gameobject);

	attackBoxTrigger = hitBoxAttackGameObject->GetComponent<ComponentBoxTrigger>();
	if (attackBoxTrigger == nullptr)
	{
		LOG("The GameObject %s has no boxTrigger component attached \n", hitBoxAttackGameObject->name);
	}

	hpHitBoxTrigger = gameobject->GetComponent<ComponentBoxTrigger>();
	if (hpHitBoxTrigger == nullptr)
	{
		LOG("The GameObject %s has no boxTrigger component attached \n", gameobject->name);
	}

	transform = gameobject->GetComponent<ComponentTransform>();
	if (transform == nullptr)
	{
		LOG("The GameObject %s has no transform component attached \n", gameobject->name);
	}

	GameObject* lifeUIGameObject = App->scene->FindGameObjectByName("Life");
	assert(lifeUIGameObject != nullptr);

	lifeUIComponent = lifeUIGameObject->GetComponent<ComponentImage>();
	assert(lifeUIComponent != nullptr);

	GameObject* manaUIGameObject = App->scene->FindGameObjectByName("Mana");
	assert(manaUIGameObject != nullptr);

	manaUIComponent = manaUIGameObject->GetComponent<ComponentImage>();
	assert(manaUIComponent != nullptr);

	GameObject* hubCooldownGO = nullptr;

	hubCooldownGO = App->scene->FindGameObjectByName("Q_Cooldown");
	if (hubCooldownGO != nullptr)
	{
		hubCooldownMask[HUB_BUTTON_Q] = hubCooldownGO->GetComponent<ComponentImage>();
		assert(hubCooldownMask[HUB_BUTTON_Q] != nullptr);
	}
	else
	{
		LOG("The Game Object 'Q_Cooldown' couldn't be found.");
	}


	hubCooldownGO = App->scene->FindGameObjectByName("W_Cooldown");
	if (hubCooldownGO != nullptr)
	{
		hubCooldownMask[HUB_BUTTON_W] = hubCooldownGO->GetComponent<ComponentImage>();
		assert(hubCooldownMask[HUB_BUTTON_W] != nullptr);
	}
	else
	{
		LOG("The Game Object 'W_Cooldown' couldn't be found.");
	}

	hubCooldownGO = App->scene->FindGameObjectByName("E_Cooldown");
	if (hubCooldownGO != nullptr)
	{
		hubCooldownMask[HUB_BUTTON_E] = hubCooldownGO->GetComponent<ComponentImage>();
		assert(hubCooldownMask[HUB_BUTTON_E] != nullptr);
	}
	else
	{
		LOG("The Game Object 'E_Cooldown' couldn't be found.");
	}

	hubCooldownGO = App->scene->FindGameObjectByName("R_Cooldown");
	if (hubCooldownGO != nullptr)
	{
		hubCooldownMask[HUB_BUTTON_R] = hubCooldownGO->GetComponent<ComponentImage>();
		assert(hubCooldownMask[HUB_BUTTON_R] != nullptr);
	}
	else
	{
		LOG("The Game Object 'R_Cooldown' couldn't be found.");
	}

	hubCooldownGO = App->scene->FindGameObjectByName("One_Cooldown");
	if (hubCooldownGO != nullptr)
	{
		hubCooldownMask[HUB_BUTTON_1] = hubCooldownGO->GetComponent<ComponentImage>();
		assert(hubCooldownMask[HUB_BUTTON_1] != nullptr);
	}
	else
	{
		LOG("The Game Object '1_Cooldown' couldn't be found.");
	}

	hubCooldownGO = App->scene->FindGameObjectByName("Two_Cooldown");
	if (hubCooldownGO != nullptr)
	{
		hubCooldownMask[HUB_BUTTON_2] = hubCooldownGO->GetComponent<ComponentImage>();
		assert(hubCooldownMask[HUB_BUTTON_2] != nullptr);
	}
	else
	{
		LOG("The Game Object '2_Cooldown' couldn't be found.");
	}

	hubCooldownGO = App->scene->FindGameObjectByName("Three_Cooldown");
	if (hubCooldownGO != nullptr)
	{
		hubCooldownMask[HUB_BUTTON_3] = hubCooldownGO->GetComponent<ComponentImage>();
		assert(hubCooldownMask[HUB_BUTTON_3] != nullptr);
	}
	else
	{
		LOG("The Game Object '3_Cooldown' couldn't be found.");
	}

	hubCooldownGO = App->scene->FindGameObjectByName("Four_Cooldown");
	if (hubCooldownGO != nullptr)
	{
		hubCooldownMask[HUB_BUTTON_4] = hubCooldownGO->GetComponent<ComponentImage>();
		assert(hubCooldownMask[HUB_BUTTON_4] != nullptr);
	}
	else
	{
		LOG("The Game Object '4_Cooldown' couldn't be found.");
	}

	GameObject* GOtemp = App->scene->FindGameObjectByName("gotHitAudio");
	if (GOtemp != nullptr)
	{
		gotHitAudio = GOtemp->GetComponent<ComponentAudioSource>();
		assert(gotHitAudio != nullptr);
	}
	else
	{
		LOG("The Game Object 'gotHitAudio' couldn't be found.");
	}

	CreatePlayerSkills();

	if (dashFX == nullptr)
	{
		LOG("DashFX Gameobject not found");
	}
	else
	{
		LOG("DashFX found");
		dashFX->SetActive(false);
		dash->dashFX = dashFX;
	}

	if (dashMesh == nullptr)
	{
		LOG("DashMesh Gameobject not found");
	}
	else
	{
		LOG("DashMesh found");
		dashMesh->SetActive(false);
		dash->meshOriginalScale = dashMesh->transform->scale;
		dash->dashMesh = dashMesh;

	}
	GOtemp = App->scene->FindGameObjectByName("PlayerMesh");
	dash->playerRenderer = GOtemp->GetComponent<ComponentRenderer>();

	bombDropMesh1 = App->scene->FindGameObjectByName("BombDropMesh1");

	if (bombDropMesh1)
	{
		bombDropMesh1Scale = bombDropMesh1->transform->scale;
	}
	else
	{
		LOG("BombDropMesh not found");
	}

	bombDropMesh2 = App->scene->FindGameObjectByName("BombDropMesh2");

	if (bombDropMesh2)
	{
		bombDropMesh2Scale = bombDropMesh2->transform->scale;
	}
	else
	{
		LOG("BombDropMesh not found");
	}

	bombDropParticlesLanding = App->scene->FindGameObjectByName("BombDropParticlesLanding");

	slashTrail = App->scene->FindGameObjectByName("SlashTrail");

	if (!slashTrail)
	{
		LOG("SlashTrail not found");
	}

	if (PlayerPrefs::HasKey("dexterity"))
	{
		stats.dexterity = PlayerPrefs::GetFloat("dexterity");
	}
	if (PlayerPrefs::HasKey("health"))
	{
		stats.health = PlayerPrefs::GetFloat("health");
	}
	if (PlayerPrefs::HasKey("hpRegen"))
	{
		stats.hpRegen = PlayerPrefs::GetFloat("hpRegen");
	}
	if (PlayerPrefs::HasKey("mana"))
	{
		stats.mana = PlayerPrefs::GetFloat("mana");
	}
	if (PlayerPrefs::HasKey("manaRegen"))
	{
		stats.manaRegen = PlayerPrefs::GetFloat("manaRegen");
	}
	if (PlayerPrefs::HasKey("strength"))
	{
		stats.strength = PlayerPrefs::GetFloat("strength");
	}
	
	InitializeUIStatsObjects();
	LOG("Started player movement script");
}

void PlayerMovement::Update()
{
	if (App->time->gameTimeScale == 0) return;

	if (health <= 0.f)
	{
		currentState = (PlayerState*)death;
		return;
	}

	PlayerState* previous = currentState;

	//Check input here and update the state!
	if (currentState != death)
	{
		for (auto it = allSkills.begin(); it != allSkills.end(); ++it) it->second->Update(App->time->fullGameDeltaTime);

		// Update cooldowns
		if (hubCooldownMask != nullptr)
		{
			/*for (unsigned i = HUB_BUTTON_1; i <= HUB_BUTTON_4; ++i)
			{
				if (hubCooldownMask[i] != nullptr && hubCooldownMask[i]->enabled)
					hubCooldownMask[i]->SetMaskAmount((int)(100.0F * hubCooldownTimer[i] / hubCooldownMax[i]));
			}*/
			for (unsigned i = HUB_BUTTON_RC; i <= HUB_BUTTON_R; ++i)
			{
				if (hubCooldownMask[i] != nullptr && hubCooldownMask[i]->enabled)
					hubCooldownMask[i]->SetMaskAmount((int)(100.0F * allSkills[assignedSkills[i]]->CooldownRatio()));
			}
		}

		currentState->UpdateTimer();

		currentState->CheckInput();

		currentState->Update();

		//if previous and current are different the functions Exit() and Enter() are called
		CheckStates(previous, currentState);	
	}

	ManaManagement();

	if (outCombatTimer > 0)
	{
		outCombatTimer -= App->time->gameDeltaTime;
	}
	else if (health < stats.health)
	{
		health += stats.hpRegen * App->time->gameDeltaTime;
		if (health > stats.health) health = stats.health;
		int healthPercentage = (health / stats.health) * 100;
		lifeUIComponent->SetMaskAmount(healthPercentage);
	}
	if (bombDropExpanding && bombDropMesh1 && bombDropMesh2)
	{
		if (bombDropMesh1->transform->scale.x < MAX_BOMB_DROP_SCALE)
		{
			bombDropMesh1->transform->Scale(bombDropGrowRate);
			shaking = false;
		}
		else
		{
			bombDropMesh1->transform->Rotate(math::float3(0.0f, BOMB_DROP_ROT, 0.0f));
			if (!shaking && playerCamera)
			{
				shaking = true;
				playerCamera->GetComponent<CameraController>()->Shake(0.5f, 85.f);
			}
		}
		if (bombDropMesh2->transform->scale.x < MAX_BOMB_DROP_WAVE_SCALE)
		{
			bombDropMesh2->transform->Scale(bombDropWaveGrowRate);
		}
		else
		{
			bombDropMesh2->SetActive(false);
		}
	}

	if (macheteRainActivated && rain->machetes)
	{
		if (rain->machetes && macheteRainRenderer && macheteRainParticles)
		{
			if (rain->machetes->transform->GetGlobalPosition().y > rain->targetHeight)
			{
				rain->machetes->transform->SetGlobalPosition(rain->machetes->transform->GetGlobalPosition() - math::float3(0, MACHETE_RAIN_SPEED * App->time->gameDeltaTime, 0));
				macheteRainRenderer->dissolveAmount = 0.f;
				shaking = false;
			}
			else
			{
				macheteRainRenderer->dissolveAmount += 1.0f * App->time->gameDeltaTime;
				macheteRainParticles->SetActive(false);
				if (!shaking && playerCamera)
				{
					shaking = true;
					playerCamera->GetComponent<CameraController>()->Shake(0.5f, 25.f);
				}
			}
		}
	}
		

	//Check for changes in the state to send triggers to animation SM
}

PlayerMovement_API void PlayerMovement::Damage(float amount)
{
	if (!isPlayerDead)
	{
		if(gotHitAudio != nullptr)
			gotHitAudio->Play();
		outCombatTimer = outCombatMaxTime;
		health -= amount;
		if (health < 0)
		{
			isPlayerDead = true;
		}

		damageController->AddDamage(gameobject->transform, amount, 5);
		if(damageUIFeedback != nullptr)
			damageUIFeedback->ActivateDamageUI();

		int healthPercentage = (health / stats.health) * 100;
		lifeUIComponent->SetMaskAmount(healthPercentage);
	}
}

void PlayerMovement::Equip(const PlayerStats & equipStats)
{
	this->stats += equipStats;

	int healthPercentage = (health / stats.health) * 100;
	lifeUIComponent->SetMaskAmount(healthPercentage);

	int manaPercentage = (mana / stats.mana) * 100;
	manaUIComponent->SetMaskAmount(manaPercentage);

	UpdateUIStats();
}

void PlayerMovement::UnEquip(const PlayerStats & equipStats)
{
	this->stats -= equipStats;
	health = health > stats.health ? stats.health : health;
	mana = mana > stats.mana ? stats.mana : mana;

	int healthPercentage = (health / stats.health) * 100;
	lifeUIComponent->SetMaskAmount(healthPercentage);

	int manaPercentage = (mana / stats.mana) * 100;
	manaUIComponent->SetMaskAmount(manaPercentage);

	UpdateUIStats();
}

void PlayerMovement::OnAnimationEvent(std::string name)
{
	if (name == "BombDropLanding")
	{
		if (bombDropParticles)
		{
			bombDropParticles->SetActive(false);
		}
		if (bombDropMesh1 && bombDropMesh2)
		{
			bombDropMesh1->SetActive(true);
			bombDropMesh2->SetActive(true);
		}
		if (bombDropParticlesLanding)
		{
			bombDropParticlesLanding->SetActive(true);
		}
		bombDropExpanding = true;
	}
	if (name == "BombDropApex")
	{
		bombDropParticles->SetActive(true);		
	}
	if (name == "BombDropEnd")
	{
		bombDropExpanding = false;
		if (bombDropMesh1 && bombDropMesh2)
		{
			bombDropMesh1->transform->scale = bombDropMesh1Scale;
			bombDropMesh1->transform->Scale(1.0f);
			bombDropMesh1->SetActive(false);
			bombDropMesh2->transform->scale = bombDropMesh2Scale;
			bombDropMesh2->transform->Scale(1.0f);
			bombDropMesh2->SetActive(false);
		}
		if (bombDropParticlesLanding)
		{
			bombDropParticlesLanding->SetActive(false);
		}
	}
	if (name == "SlashStart" && slashTrail)
	{
		LOG("SlashStart");
		slashTrail->SetActive(true);
	}
	if ((name == "SlashEnd" || name == "SlashBegin") && slashTrail)
	{
		LOG("SlashEnd");
		slashTrail->SetActive(false);
	}

}

void PlayerMovement::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddFloat("General_Ability_Cooldown", hubGeneralAbilityCooldown);
	json->AddFloat("RC_Cooldown", hubCooldown[HUB_BUTTON_RC]);
	json->AddFloat("1_Cooldown", hubCooldown[HUB_BUTTON_1]);
	json->AddFloat("2_Cooldown", hubCooldown[HUB_BUTTON_2]);
	json->AddFloat("3_Cooldown", hubCooldown[HUB_BUTTON_3]);
	json->AddFloat("4_Cooldown", hubCooldown[HUB_BUTTON_4]);
	json->AddFloat("Q_Cooldown", hubCooldown[HUB_BUTTON_Q]);
	json->AddFloat("W_Cooldown", hubCooldown[HUB_BUTTON_W]);
	json->AddFloat("E_Cooldown", hubCooldown[HUB_BUTTON_E]);
	json->AddFloat("R_Cooldown", hubCooldown[HUB_BUTTON_R]);
	json->AddUint("Show_Ability_Cooldown", showAbilityCooldowns ? 1 : 0);
	json->AddUint("Show_Items_Cooldown", showItemCooldowns ? 1 : 0);
	json->AddFloat("Out_of_combat_timer", outCombatMaxTime);

	json->AddFloat("walkingSpeed", walkingSpeed);
	json->AddFloat("MeshCorrectionXZ", OutOfMeshCorrectionXZ);
	json->AddFloat("MeshCorrectionY", OutOfMeshCorrectionY);
	json->AddFloat("MaxWalkDistance", maxWalkingDistance);
	json->AddFloat("StraightPathDistance", straightPathingDistance);

	JSON_value* keyboard_abilities = json->CreateValue();
	keyboard_abilities->AddInt("RC", (int)assignedSkills[HUB_BUTTON_RC]);
	keyboard_abilities->AddInt("1", (int)assignedSkills[HUB_BUTTON_1]);
	keyboard_abilities->AddInt("2", (int)assignedSkills[HUB_BUTTON_1]);
	keyboard_abilities->AddInt("3", (int)assignedSkills[HUB_BUTTON_1]);
	keyboard_abilities->AddInt("4", (int)assignedSkills[HUB_BUTTON_1]);
	keyboard_abilities->AddInt("Q", (int)assignedSkills[HUB_BUTTON_Q]);
	keyboard_abilities->AddInt("W", (int)assignedSkills[HUB_BUTTON_W]);
	keyboard_abilities->AddInt("E", (int)assignedSkills[HUB_BUTTON_E]);
	keyboard_abilities->AddInt("R", (int)assignedSkills[HUB_BUTTON_R]);
	json->AddValue("keyboard_abilities", *keyboard_abilities);

	JSON_value* abilities = json->CreateValue();
	{
		JSON_value* chain_data = json->CreateValue();
		if (allSkills.find(SkillType::CHAIN) != allSkills.end()) allSkills.find(SkillType::CHAIN)->second->Serialize(chain_data);
		abilities->AddValue("chain", *chain_data);
	}
	{
		JSON_value* dash_data = json->CreateValue();
		if (allSkills.find(SkillType::DASH) != allSkills.end()) allSkills.find(SkillType::DASH)->second->Serialize(dash_data);
		abilities->AddValue("dash", *dash_data);
	}
	{
		JSON_value* slice_data = json->CreateValue();
		if (allSkills.find(SkillType::SLICE) != allSkills.end()) allSkills.find(SkillType::SLICE)->second->Serialize(slice_data);
		abilities->AddValue("slice", *slice_data);
	}
	{
		JSON_value* bombDrop_data = json->CreateValue();
		if (allSkills.find(SkillType::BOMB_DROP) != allSkills.end()) allSkills.find(SkillType::BOMB_DROP)->second->Serialize(bombDrop_data);
		abilities->AddValue("bombDrop", *bombDrop_data);
	}
	{
		JSON_value* circular_data = json->CreateValue();
		if (allSkills.find(SkillType::CIRCULAR) != allSkills.end()) allSkills.find(SkillType::CIRCULAR)->second->Serialize(circular_data);
		abilities->AddValue("circular", *circular_data);
	}
	{
		JSON_value* stomp_data = json->CreateValue();
		if (allSkills.find(SkillType::STOMP) != allSkills.end()) allSkills.find(SkillType::STOMP)->second->Serialize(stomp_data);
		abilities->AddValue("stomp", *stomp_data);
	}
	{
		JSON_value* rain_data = json->CreateValue();
		if (allSkills.find(SkillType::RAIN) != allSkills.end()) allSkills.find(SkillType::RAIN)->second->Serialize(rain_data);
		abilities->AddValue("rain", *rain_data);
	}
	json->AddValue("abilities", *abilities);

	stats.Serialize(json);
}

void PlayerMovement::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	hubGeneralAbilityCooldown = json->GetFloat("General_Ability_Cooldown", 0.5F);
	hubCooldown[HUB_BUTTON_RC] = json->GetFloat("RC_Cooldown", 1.0F);
	hubCooldown[HUB_BUTTON_1] = json->GetFloat("1_Cooldown", 1.0F);
	hubCooldown[HUB_BUTTON_2] = json->GetFloat("2_Cooldown", 1.0F);
	hubCooldown[HUB_BUTTON_3] = json->GetFloat("3_Cooldown", 1.0F);
	hubCooldown[HUB_BUTTON_4] = json->GetFloat("4_Cooldown", 1.0F);
	hubCooldown[HUB_BUTTON_Q] = json->GetFloat("Q_Cooldown", 1.0F);
	hubCooldown[HUB_BUTTON_W] = json->GetFloat("W_Cooldown", 1.0F);
	hubCooldown[HUB_BUTTON_E] = json->GetFloat("E_Cooldown", 1.0F);
	hubCooldown[HUB_BUTTON_R] = json->GetFloat("R_Cooldown", 1.0F);

	showAbilityCooldowns = json->GetUint("Show_Ability_Cooldown", 1U) == 1;
	showItemCooldowns = json->GetUint("Show_Items_Cooldown", 1U) == 1;

	walkingSpeed = json->GetFloat("walkingSpeed", 300.0f);
	OutOfMeshCorrectionXZ = json->GetFloat("MeshCorrectionXZ", 500.f);
	OutOfMeshCorrectionY = json->GetFloat("MeshCorrectionY", 300.f);
	maxWalkingDistance = json->GetFloat("MaxWalkDistance", 50000.0f);
	straightPathingDistance = json->GetFloat("StraightPathDistance", 3000.0f);

	outCombatMaxTime = json->GetFloat("Out_of_combat_timer", 3.f);

	JSON_value* keyboard_abilities = json->GetValue("keyboard_abilities");
	if (keyboard_abilities)
	{
		assignedSkills[HUB_BUTTON_RC] = (SkillType)keyboard_abilities->GetInt("RC");
		//assignedSkills[HUB_BUTTON_1] = (SkillType)keyboard_abilities->GetInt("1");
		//assignedSkills[HUB_BUTTON_2] = (SkillType)keyboard_abilities->GetInt("2");
		//assignedSkills[HUB_BUTTON_3] = (SkillType)keyboard_abilities->GetInt("3");
		//assignedSkills[HUB_BUTTON_4] = (SkillType)keyboard_abilities->GetInt("4");
		assignedSkills[HUB_BUTTON_Q] = (SkillType)keyboard_abilities->GetInt("Q");
		assignedSkills[HUB_BUTTON_W] = (SkillType)keyboard_abilities->GetInt("W");
		assignedSkills[HUB_BUTTON_E] = (SkillType)keyboard_abilities->GetInt("E");
		assignedSkills[HUB_BUTTON_R] = (SkillType)keyboard_abilities->GetInt("R");
	}

	JSON_value* abilities = json->GetValue("abilities");
	if (abilities)
	{
		JSON_value* stomp_data = abilities->GetValue("stomp");
		if (stomp_data) allSkills[SkillType::STOMP]->DeSerialize(stomp_data, stomp);

		JSON_value* chain_data = abilities->GetValue("chain");
		if (chain_data) allSkills[SkillType::CHAIN]->DeSerialize(chain_data, chain);

		JSON_value* dash_data = abilities->GetValue("dash");
		if (dash_data) allSkills[SkillType::DASH]->DeSerialize(dash_data, dash);

		JSON_value* slice_data = abilities->GetValue("slice");
		if (slice_data) allSkills[SkillType::SLICE]->DeSerialize(slice_data, slice);

		JSON_value* bombDrop_data = abilities->GetValue("bombDrop");
		if (bombDrop_data) allSkills[SkillType::BOMB_DROP]->DeSerialize(bombDrop_data, bombDrop);

		JSON_value* circular_data = abilities->GetValue("circular");
		if (circular_data) allSkills[SkillType::CIRCULAR]->DeSerialize(circular_data, circular);

		JSON_value* rain_data = abilities->GetValue("rain");
		if (rain_data) allSkills[SkillType::CIRCULAR]->DeSerialize(rain_data, rain);
	}


	stats.DeSerialize(json);
}

void PlayerMovement::OnTriggerExit(GameObject* go)
{

}

bool PlayerMovement::IsAtacking() const
{
	return !App->ui->UIHovered(true,false) && App->input->GetMouseButtonDown(1) == KEY_DOWN; //Left button
}

bool PlayerMovement::IsMoving() const
{
	math::float3 temp;
	return ( (App->input->GetMouseButtonDown(3) == KEY_DOWN && !App->ui->UIHovered(false, true)) ||
			 (currentState->playerWalking) || 
			 (App->input->GetMouseButtonDown(3) == KEY_REPEAT && !App->ui->UIHovered(false, true) && !App->scene->Intersects("PlayerMesh", false, temp))); //right button, the player is still walking or movement button is pressed and can get close to mouse pos
}

bool PlayerMovement::IsUsingOne() const
{
	return allSkills.find(assignedSkills[HUB_BUTTON_1])->second->IsUsable(mana) && App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN;
}

bool PlayerMovement::IsUsingTwo() const
{
	return allSkills.find(assignedSkills[HUB_BUTTON_2])->second->IsUsable(mana) && App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN;
}

bool PlayerMovement::IsUsingThree() const
{
	return allSkills.find(assignedSkills[HUB_BUTTON_3])->second->IsUsable(mana) && App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN;
}

bool PlayerMovement::IsUsingFour() const
{
	return allSkills.find(assignedSkills[HUB_BUTTON_4])->second->IsUsable(mana) && App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN;
}

bool PlayerMovement::IsUsingQ() const
{
	return allSkills.find(assignedSkills[HUB_BUTTON_Q])->second->IsUsable(mana) && App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN;
}

bool PlayerMovement::IsUsingW() const
{
	return allSkills.find(assignedSkills[HUB_BUTTON_W])->second->IsUsable(mana) && App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN;
}

bool PlayerMovement::IsUsingE() const
{
	return allSkills.find(assignedSkills[HUB_BUTTON_E])->second->IsUsable(mana) && App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN;
}

bool PlayerMovement::IsUsingR() const
{
	return allSkills.find(assignedSkills[HUB_BUTTON_R])->second->IsUsable(mana) && App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN;
}

bool PlayerMovement::IsUsingSkill() const
{
	return (IsUsingOne() || IsUsingTwo() || IsUsingThree() || IsUsingFour()|| IsUsingQ() || IsUsingW() || IsUsingE() || IsUsingR());
}

void PlayerMovement::UseSkill(SkillType skill)
{
	manaRegenTimer = manaRegenMaxTime;
	for (auto it = allSkills.begin(); it != allSkills.end(); ++it)
	{
		if (it->second->type == skill)
		{
			mana -= it->second->Use(hubGeneralAbilityCooldown);
		}
		else
		{
			it->second->SetCooldown(hubGeneralAbilityCooldown);
		}
	}

	for (unsigned i = 0; i < 4; ++i)
	{
		hubCooldownTimer[i] = allSkills[assignedSkills[i]]->cooldown;
		hubCooldownMax[i] = allSkills[assignedSkills[i]]->cooldown;
	}
}

void PlayerMovement::ResetCooldown(unsigned int hubButtonID)
{
	if (hubButtonID <= HUB_BUTTON_R)
	{
		for (unsigned i = HUB_BUTTON_RC; i <= HUB_BUTTON_R; ++i)
		{
			hubCooldownTimer[i] = hubGeneralAbilityCooldown;
			hubCooldownMax[i] = hubGeneralAbilityCooldown;
		}
	}

	if (hubCooldownTimer[hubButtonID] < hubCooldown[hubButtonID])
	{
		hubCooldownTimer[hubButtonID] = hubCooldown[hubButtonID];
		hubCooldownMax[hubButtonID] = hubCooldown[hubButtonID];
	}
}

void PlayerMovement::CheckStates(PlayerState * previous, PlayerState * current)
{
	if (previous != current)
	{
		previous->ResetTimer();

		previous->Exit();
		current->Enter();

		if (anim != nullptr)
		{
			anim->SendTriggerToStateMachine(current->trigger.c_str());
		}

		current->duration = anim->GetDurationFromClip();
	}
}

void PlayerMovement::ManaManagement()
{
	if (manaRegenTimer > 0)
	{
		manaRegenTimer -= App->time->gameDeltaTime;
	}
	else if (mana < stats.mana && outCombatTimer <= 0)
	{
		mana += stats.manaRegen * App->time->gameDeltaTime;
		if (mana > stats.mana) mana = stats.mana;
	}

	int manaPercentage = (mana / stats.mana) * 100;
	manaUIComponent->SetMaskAmount(manaPercentage);
}

void PlayerStats::Serialize(JSON_value * json) const
{
	JSON_value* statsValue = json->CreateValue();

	statsValue->AddFloat("health", health);
	statsValue->AddFloat("mana", mana);
	statsValue->AddInt("strength", strength);
	statsValue->AddInt("dexterity", dexterity);
	statsValue->AddFloat("hp_regen", hpRegen);
	statsValue->AddFloat("mana_regen", manaRegen);

	json->AddValue("stats", *statsValue);
}

void PlayerStats::DeSerialize(JSON_value * json)
{

	JSON_value* statsValue = json->GetValue("stats");
	if (!statsValue) return;

	health = statsValue->GetFloat("health", 100.0F);
	mana = statsValue->GetFloat("mana", 100.0F);
	strength = statsValue->GetInt("strength", 10);
	dexterity = statsValue->GetInt("dexterity", 10);
	hpRegen = statsValue->GetFloat("hp_regen", 5.0F);
	manaRegen = statsValue->GetFloat("mana_regen", 5.0F);
}

void PlayerStats::Expose(const char* sectionTitle)
{
	ImGui::Text(sectionTitle);
	ImGui::InputFloat("Health", &health);
	ImGui::InputFloat("Mana", &mana);

	int uiStrength = (int)strength;
	if (ImGui::InputInt("Strength", &uiStrength)) strength = uiStrength < 0 ? 0 : uiStrength;

	int uiDexterity = (int)dexterity;
	if (ImGui::InputInt("Dexterity", &uiDexterity)) dexterity = uiDexterity < 0 ? 0 : uiDexterity;
	
	ImGui::DragFloat("HP regen", &hpRegen, 1.0F, 0.0F, 10.0F);
	ImGui::DragFloat("Mana regen", &manaRegen, 1.0F, 0.0F, 10.0F);
}

void PlayerMovement::ActivateHudCooldownMask(bool activate, unsigned first, unsigned last)
{
	for (unsigned i = first; i <= last; ++i) hubCooldownMask[i]->gameobject->SetActive(activate);
}

void PlayerSkill::Expose(const char* title)
{
	bool open = true;
	{
		ImGui::PushID(title);
		ImGui::Bullet(); ImGui::SameLine(); ImGui::Text(title);
		ImGui::DragFloat("Mana Cost", &manaCost);
		ImGui::DragFloat("Cooldown", &this->cooldown);
		ImGui::Text("Timer: %f (%f)", cooldownTimer, CooldownRatio());
		ImGui::PopID();
	}
}

void PlayerSkill::Serialize(JSON_value* json) const
{
	json->AddInt("type", (int)type);
	json->AddFloat("mana_cost", manaCost);
	json->AddFloat("cooldown", cooldown);
}

void PlayerSkill::DeSerialize(JSON_value* json, BasicSkill* playerSkill)
{
	//type = (SkillType)json->GetInt("type"); 
	manaCost = json->GetFloat("mana_cost");
	cooldown = json->GetFloat("cooldown");
	skill = playerSkill;
}

void PlayerMovement::UpdateUIStats()
{
	if (uiHealthText != nullptr && uiDexterityText != nullptr && uiStrengthText != nullptr && uiManaText != nullptr)
	{
		uiHealthText->text = std::to_string((int)stats.health);
		uiDexterityText->text = std::to_string(stats.dexterity);
		uiStrengthText->text = std::to_string(stats.strength);
		uiManaText->text = std::to_string((int)stats.mana);
	}
}

void PlayerMovement::InitializeUIStatsObjects()
{
	GameObject* statsPanel = App->scene->FindGameObjectByName("StatsPanel");

	if (statsPanel != nullptr)
	{
		GameObject* heartPanel = App->scene->FindGameObjectByName("HeartPanel", statsPanel);
		if (heartPanel != nullptr)
		{
			uiHealthText = App->scene->FindGameObjectByName("HeartLabel", heartPanel)->GetComponent<Text>();
		}
		else
		{
			LOG("The Game Object 'HeartPanel' couldn't be found.");
		}

		GameObject* dexterityPanel = App->scene->FindGameObjectByName("DexterityPanel", statsPanel);
		if (dexterityPanel != nullptr)
		{
			uiDexterityText = App->scene->FindGameObjectByName("DexterityLabel", dexterityPanel)->GetComponent<Text>();
		}
		else
		{
			LOG("The Game Object 'DexterityPanel' couldn't be found.");
		}

		GameObject* strPanel = App->scene->FindGameObjectByName("StrPanel", statsPanel);
		if (strPanel != nullptr)
		{
			uiStrengthText = App->scene->FindGameObjectByName("StrLabel", strPanel)->GetComponent<Text>();
		}
		else
		{
			LOG("The Game Object 'StrPanel' couldn't be found.");
		}

		GameObject* soulPanel = App->scene->FindGameObjectByName("SoulPanel", statsPanel);
		if (soulPanel != nullptr)
		{
			uiManaText = App->scene->FindGameObjectByName("SoulLabel", soulPanel)->GetComponent<Text>();
		}
		else
		{
			LOG("The Game Object 'SoulPanel' couldn't be found.");
		}

		UpdateUIStats();
	}
	else
	{
		LOG("The Game Object 'StatsPanel' couldn't be found.");
	}
}

void PlayerMovement::SetGodMode(bool active)
{
	if (active)
	{
		PlayerStats godStats = {400.f, 999.f, 999.f, 999.f, 999.9f, 999.9f};
		previousStats = stats;
		stats = godStats;
		health = 10000.f;
		mana = 10000.f;
	}
	else
	{
		stats = previousStats;
		health = 100.f;
		mana = 100.f;
	}
}

void PlayerMovement::SavePlayerStats()
{
	PlayerPrefs::SetFloat("dexterity", stats.dexterity);
	PlayerPrefs::SetFloat("health", stats.health);
	PlayerPrefs::SetFloat("hpRegen", stats.hpRegen);
	PlayerPrefs::SetFloat("mana", stats.mana);
	PlayerPrefs::SetFloat("manaRegen", stats.manaRegen);
	PlayerPrefs::SetFloat("strength", stats.strength);
}
