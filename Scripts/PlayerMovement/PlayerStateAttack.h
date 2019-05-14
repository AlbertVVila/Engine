#ifndef __PLAYERSTATEATTACK_H_
#define __PLAYERSTATEATTACK_H_

#include "PlayerState.h"

#ifdef PlayerMovement_EXPORTS
#define PlayerMovement_API __declspec(dllexport)

#else
#define PlayerMovement_API __declspec(dllimport)
#endif

class PlayerMovement_API PlayerStateAttack :
	public PlayerState
{
public:
	PlayerStateAttack();
	~PlayerStateAttack();

	void Update() override;
};

#endif // __PLAYERSTATEATTACK_H_