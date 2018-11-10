#ifndef __ModuleModel_h__
#define __ModuleModel_h__

#include "Module.h"
#include "Model.h"
#include <list>

struct Texture;

class ModuleModel :
	public Module
{
public:
	ModuleModel();
	~ModuleModel();

	bool Init();
	unsigned int Load(const char *path);
	void DrawModels();
	void DrawModelProperties();
	void DrawGUI();
	void ApplyTexture(Texture texture);
	void DeleteModels();

	bool CleanUp();

	std::list<Model> models;
	bool checkers = false;
	Texture checkersTexture = Texture(0,0,0);
};

#endif //__ModuleModel_h__
