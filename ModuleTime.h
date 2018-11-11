#ifndef __ModuleTime_h__
#define __ModuleTime_h__

#include "Module.h"
#include "SDL_timer.h"
class ModuleTime : public Module
{
public:
	ModuleTime();
	~ModuleTime();

	bool            Init();
	update_status   PreUpdate();

public:
	double deltaTime = 0.0f;	// Time between current frame and last frame
	Uint64 before = 0; // Time of last frame
};

#endif /* __ModuleTime_h__ */

