#ifndef __ModuleScript_h__
#define __ModuleScript_h__

#include "Module.h"
#include <vector>
 
class Script;

class ModuleScript :
	public Module
{
public:

	ModuleScript();
	~ModuleScript();

	bool Start() override;
	update_status Update(float dt) override;

	Script* AddScript(std::string script);
	void RemoveScript(Script* script);

public:
	std::vector<std::string> scripts;
	//std::vector<std::pair<std::string, int>> scripts; //name, modification date
private:
	std::list<Script*> scriptInstances;
};

#endif __ModuleScript_h__