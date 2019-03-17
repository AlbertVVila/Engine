#ifndef  __EnemyMovementScript_h__
#define  __EnemyMovementScript_h__

class GameObject;
class JSON_value;
struct ImGuiContext;
#include "BaseScript.h"
#include "Math/float3.h"

#ifdef EnemyMovementScript_EXPORTS
#define EnemyMovementScript_API __declspec(dllexport)
#else
#define EnemyMovementScript_API __declspec(dllimport)
#endif

class EnemyMovementScript_API EnemyMovementScript : public Script
{
public:
	void Expose(ImGuiContext* context) override;

	void Start() override;
	void Update() override;
	//void Serialize(JSON_value* json) const override;
	//void DeSerialize(JSON_value* json) override;
private:
	GameObject* GetGameObjectByName(GameObject* gameObject, const char* name);
private:
	float speed = 0.5f;
	std::string playerName = "Player";
	GameObject* player = nullptr;
};

extern "C" EnemyMovementScript_API EnemyMovementScript* CreateScript();

#endif __EnemyMovementScript_h__
