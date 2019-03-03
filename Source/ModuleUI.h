#ifndef __ModuleUI_H__
#define __ModuleUI_H__

#include "Module.h"

class ModuleUI :
	public Module
{
public:
	ModuleUI();
	virtual ~ModuleUI();

	bool Init(JSON* json) override;
	update_status Update(float dt) override;
	update_status PostUpdate() override;
	bool CleanUp() override;
};

#endif // __ModuleUI_H__
