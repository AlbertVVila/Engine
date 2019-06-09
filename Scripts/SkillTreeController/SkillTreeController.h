#ifndef  __SkillTreeController_h__
#define  __SkillTreeController_h__

#include "BaseScript.h"

#ifdef SkillTreeController_EXPORTS
#define SkillTreeController_API __declspec(dllexport)
#else
#define SkillTreeController_API __declspec(dllimport)
#endif

class GameObject;
class Text;

class SkillTreeController_API SkillTreeController : public Script
{

	void Start() override;
	void Update() override;

	void AddSkillPoint();

private:
	int skillPoints = 0;

	GameObject* skills = nullptr;
	GameObject* tree = nullptr;
	Text* skillPointsLabel = nullptr;
};

#endif __SkillTreeController_h__
