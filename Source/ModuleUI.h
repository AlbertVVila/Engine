#ifndef __ModuleUI_H__
#define __ModuleUI_H__

#include "Module.h"

struct Shader; 
struct Texture;

class ComponentCamera;

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

	void Draw(const ComponentCamera &camera);
private:
	const char* shaderFile = "UI";
	Shader* shader = nullptr;

	unsigned IBO = 0;
	unsigned VBO = 0;
	unsigned UBO = 0;
	
	Texture* texture = nullptr;
};

#endif // __ModuleUI_H__
