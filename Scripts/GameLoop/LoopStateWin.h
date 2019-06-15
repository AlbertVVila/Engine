#ifndef __LOOPSTATEWIN_H_
#define __LOOPSTATEWIN_H_

#include "LoopState.h"

class LoopStateWin : public LoopState
{
public:
	LoopStateWin(GameLoop* GL);
	~LoopStateWin();

	void Update() override;
};

#endif // __LOOPSTATEWIN_H_