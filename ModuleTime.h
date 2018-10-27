#ifndef __ModuleTime_h__
#define __ModuleTime_h__

#include "Module.h"

class ModuleTime : public Module
{
public:
	ModuleTime();
	~ModuleTime();

	bool            Init();
	update_status   PreUpdate();

public:
	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame
};

#endif /* __ModuleTime_h__ */

