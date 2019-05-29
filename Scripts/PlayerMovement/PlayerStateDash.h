#ifndef __PLAYERSTATEDASH_H_
#define __PLAYERSTATEDASH_H_

#include "PlayerState.h"

class PlayerStateDash :
	public PlayerState
{
public:
	PlayerStateDash(PlayerMovement* PM);
	~PlayerStateDash();

	void Update() override;
	void Enter() override;
	void CheckInput() override;

public:
	math::float3 intersectionPoint;
	std::vector<float3>path;
	unsigned pathIndex = 0;
	float duration = 0.8f;
	float dashSpeed = 900.0f;
	float dashPreparationTime = 0.1f;

	GameObject* dashFX = nullptr;
	GameObject* dashMesh = nullptr;
	math::float3 meshOriginalScale = math::float3::one;

	float originalScalator = 1.05f;
	float scalator = originalScalator;
	float scalatorDecay = 0.006f;
};

#endif // __PLAYERSTATEDASH_H_