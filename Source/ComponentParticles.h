#ifndef __ComponentParticles_h__
#define __ComponentParticles_h__

#include "Component.h"
#include <string>

struct Texture;

class ComponentParticles :
	public Component
{
	friend class ModuleParticles;

public:
	ComponentParticles(GameObject* gameobject);
	ComponentParticles(const ComponentParticles& component);
	~ComponentParticles();


	Component* Clone() const override;
	void DrawProperties() override;
	bool CleanUp() override;

	void Update(float dt);

	void Save(JSON_value* value) const override;
	void Load(JSON_value* value) override;

	Texture* texture = nullptr;

private:
	std::string textureName = "None Selected";
	std::vector<std::string> textureFiles;

	int xTiles = 1;
	int yTiles = 1;
	float fps = 24.f;

	float timer = 0.f;
	float frameMix = 0.f;
	int f1Xpos;
	int f1Ypos;
	int f2Xpos;
	int f2Ypos;
};

#endif __ComponentParticles_h__