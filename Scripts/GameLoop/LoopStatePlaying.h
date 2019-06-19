#ifndef __LOOPSTATEPLAYING_H_
#define __LOOPSTATEPLAYING_H_

#include "LoopState.h"

class Button;

class LoopStatePlaying :
	public LoopState
{
public:
	LoopStatePlaying(GameLoop* GL);
	virtual ~LoopStatePlaying();

	void HandleHotkeys();

	void Update() override;
};

#endif // __LOOPSTATEPLAYING_H_