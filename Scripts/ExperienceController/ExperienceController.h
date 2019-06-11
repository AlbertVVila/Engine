#ifndef  __ExperienceController_h__
#define  __ExperienceController_h__

#include "BaseScript.h"

#ifdef ExperienceController_EXPORTS
#define ExperienceController_API __declspec(dllexport)
#else
#define ExperienceController_API __declspec(dllimport)
#endif

class Text;
class ComponentImage;
class SkillTreeController;
class GameObject;

class ExperienceController_API ExperienceController : public Script
{
	void Start() override;
	void Update() override;

	void AddXP(int xp);

private:
	Text* xpText = nullptr;
	Text* levelText = nullptr;
	Text* levelReached = nullptr;
	ComponentImage* xpProgressHUD = nullptr;
	ComponentImage* xpProgressInventory = nullptr;
	SkillTreeController* skillTreeScript = nullptr;
	GameObject* levelUPGO = nullptr;

	int currentXP = 0;
	int previousXP = 0;
	int currentLevel = 1;
	int maxXPLevel = 100;
	int maxLevel = 10;
	int levelsExp[10] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
	int totalXPAcumulated = 0;
	bool updateXP = false;
	bool levelUP = false;

	//Timers
	float timeShowing = 3.0f;
	float currentTime = 0.0f;
};

#endif __ExperienceController_h__
