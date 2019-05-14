#ifndef __PLAYERSTATE_H_
#define __PLAYERSTATE_H_

#include "PlayerMovement.h"

#ifdef PlayerMovement_EXPORTS
#define PlayerMovement_API __declspec(dllexport)

#else
#define PlayerMovement_API __declspec(dllimport)
#endif

class PlayerMovement_API PlayerState
{
public:
	PlayerState();
	~PlayerState();

	virtual void Update();

};

#endif // _PLAYERSTATE_H_
