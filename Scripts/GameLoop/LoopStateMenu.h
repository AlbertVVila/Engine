#ifndef __LOOPSTATEMENU_H_
#define __LOOPSTATEMENU_H_

#include "LoopState.h"

class Button;

class LoopStateMenu :
	public LoopState
{
public:
	LoopStateMenu(GameLoop* GL);
	~LoopStateMenu();

	void Update() override;
};

#endif // __LOOPSTATEMENU_H_