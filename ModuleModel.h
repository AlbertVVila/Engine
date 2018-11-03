#ifndef __ModuleModel_h__
#define __ModuleModel_h__

#include "Module.h"
#include "Model.h"
#include <list>

class ModuleModel :
	public Module
{
public:
	ModuleModel();
	~ModuleModel();

	bool Init();
	unsigned int Load(const char *path);
	void DrawModels();
	bool CleanUp();

	std::list<Model> models;
};

#endif __ModuleModel_h__
