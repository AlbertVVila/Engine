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

	void drawCanvas();

	//variables
	const char* shaderFile = "UI";
	
	Shader* shaderCanvas = nullptr;
	

	unsigned VAO = 0;
	unsigned VBO = 0;
	unsigned EBO = 0;

	Texture* texture = nullptr;
};

#endif // __ModuleUI_H__
