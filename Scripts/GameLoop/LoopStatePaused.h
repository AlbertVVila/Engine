#ifndef __LOOPSTATEPAUSED_H_
#define __LOOPSTATEPAUSED_H_

#include "LoopState.h"

class LoopStatePaused :
	public LoopState
{
public:
	LoopStatePaused(GameLoop* GL);
	~LoopStatePaused();

	void Update() override;
};

#endif // __LOOPSTATEPAUSED_H_