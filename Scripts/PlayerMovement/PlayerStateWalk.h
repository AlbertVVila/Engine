#ifndef __PLAYERSTATEWALK_H_
#define __PLAYERSTATEWALK_H_

#include "PlayerState.h"

#ifdef PlayerMovement_EXPORTS
#define PlayerMovement_API __declspec(dllexport)

#else
#define PlayerMovement_API __declspec(dllimport)
#endif

class PlayerMovement_API PlayerStateWalk :
	public PlayerState
{
public:
	PlayerStateWalk();
	~PlayerStateWalk();

	void Update() override;
};

#endif // __PLAYERSTATEWALK_H_