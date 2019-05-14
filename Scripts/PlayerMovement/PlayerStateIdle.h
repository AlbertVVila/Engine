#ifndef __PLAYERSTATEIDLE_H_
#define __PLAYERSTATEIDLE_H_

#include "PlayerState.h"

#ifdef PlayerMovement_EXPORTS
#define PlayerMovement_API __declspec(dllexport)

#else
#define PlayerMovement_API __declspec(dllimport)
#endif

class PlayerMovement_API PlayerStateIdle :
	public PlayerState
{
public:
	PlayerStateIdle();
	~PlayerStateIdle();

	void Update() override;
};

#endif // __PLAYERSTATEIDLE_H_