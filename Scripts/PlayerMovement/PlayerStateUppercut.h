#ifndef __PLAYERSTATEUPPERCUT_H_
#define __PLAYERSTATEUPPERCUT_H_

#include "PlayerState.h"

#ifdef PlayerMovement_EXPORTS
#define PlayerMovement_API __declspec(dllexport)

#else
#define PlayerMovement_API __declspec(dllimport)
#endif

class PlayerMovement_API PlayerStateUppercut :
	public PlayerState
{
public:
	PlayerStateUppercut();
	~PlayerStateUppercut();
};

#endif // __PLAYERSTATEUPPERCUT_H_