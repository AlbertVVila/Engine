#ifndef __LOOPSTATEPLAYING_H_
#define __LOOPSTATEPLAYING_H_

#include "LoopState.h"

class Button;

class LoopStatePlaying :
	public LoopState
{
public:
	LoopStatePlaying(GameLoop* GL);
	~LoopStatePlaying();

	void Update() override;
};

#endif // __LOOPSTATEPLAYING_H_