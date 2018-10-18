#ifndef __ModuleProgram_h__
#define __ModuleProgram_h__

#include "Module.h"
#include "MathGeoLib.h"

class ModuleProgram : public Module
{
public:
	ModuleProgram();
	~ModuleProgram();

	bool            Init();
	bool            CleanUp();

};

#endif /* __ModuleProgram_h__ */
