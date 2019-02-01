#ifndef __ModuleScript_h__
#define __ModuleScript_h__

#include "Module.h"
#include <list>
class Script;

class ModuleScript :
	public Module
{
public:

	ModuleScript();
	~ModuleScript();

	bool Start() override;
	update_status Update(float dt) override;

public:
	std::list<Script*> scripts;
};

#endif __ModuleScript_h__