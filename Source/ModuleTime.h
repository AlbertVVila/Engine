#ifndef __ModuleTime_h__
#define __ModuleTime_h__

#include "Module.h"
#include "SDL_timer.h"
class ModuleTime : public Module
{
public:
	ModuleTime();
	~ModuleTime();

	update_status   Update(float dt) override;

public:
	double dt = 0.0f;	// Time between current frame and last frame
	Uint64 before = 0; // Time of last frame
};

#endif /* __ModuleTime_h__ */

