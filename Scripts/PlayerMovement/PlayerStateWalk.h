#ifndef __PLAYERSTATEWALK_H_
#define __PLAYERSTATEWALK_H_

#include "PlayerState.h"

class PlayerStateWalk :	public PlayerState
{
public:
	PlayerStateWalk(PlayerMovement * PM, const char * trigger);
	~PlayerStateWalk();

	void Update() override;
	void CheckInput() override;
	void Enter() override;

public:
	float duration = 1.5f;
	std::vector<float3>path;
	unsigned pathIndex = 0;
	GameObject* dustParticles = nullptr;

private:
	math::float3 startingFront = math::float3(0.f, 0.f, 0.f);
	float currentLerping = 0.25f;
	float moveTimer = 0.0f;
};

#endif // __PLAYERSTATEWALK_H_