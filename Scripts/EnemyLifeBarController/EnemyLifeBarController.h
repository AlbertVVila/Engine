#ifndef  __EnemyLifeBarController_h__
#define  __EnemyLifeBarController_h__

#include "BaseScript.h"

#ifdef EnemyLifeBarController_EXPORTS
#define EnemyLifeBarController_API __declspec(dllexport)
#else
#define EnemyLifeBarController_API __declspec(dllimport)
#endif

#include "Math/float2.h"

class GameObject;
class Transform2D;

enum class EnemyLifeBarType
{
	NORMAL = 0,
	HARD,
	BOSS
};

class EnemyLifeBarController_API EnemyLifeBarController : public Script
{

public:
	void Start() override;
	void Update() override;

	void SetLifeBar(int maxHP, int actualHP, EnemyLifeBarType type, std::string name);

private:
	GameObject* lifeBackground = nullptr;
	GameObject* hPbar = nullptr;
	GameObject* enemyTypeName = nullptr;
	GameObject* boneRight = nullptr;
	GameObject* boneLeft = nullptr;
	GameObject* skull = nullptr;

	Transform2D* life = nullptr;

	math::float2 lifeBarMaxWidht = math::float2::zero;
	math::float2 lifeBarPositionX = math::float2::zero;
};

#endif __EnemyLifeBarController_h__
