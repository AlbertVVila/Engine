#ifndef __PLAYERSTATEDASH_H_
#define __PLAYERSTATEDASH_H_

#include "PlayerState.h"

#ifdef PlayerMovement_EXPORTS
#define PlayerMovement_API __declspec(dllexport)

#else
#define PlayerMovement_API __declspec(dllimport)
#endif

class PlayerMovement_API PlayerStateDash :
	public PlayerState
{
public:
	PlayerStateDash();
	~PlayerStateDash();

	void Update() override;
};

#endif // __PLAYERSTATEDASH_H_