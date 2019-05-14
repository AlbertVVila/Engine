#ifndef __PLAYERSTATEDEATH_H_
#define __PLAYERSTATEDEATH_H_

#include "PlayerMovement.h"
#include "PlayerState.h"


#ifdef PlayerMovement_EXPORTS
#define PlayerMovement_API __declspec(dllexport)

#else
#define PlayerMovement_API __declspec(dllimport)
#endif

class PlayerMovement_API PlayerStateDeath :
	public PlayerState
{
public:
	PlayerStateDeath();
	~PlayerStateDeath();

	void Update() override;
};

#endif // __PLAYERSTATEDEATH_H_