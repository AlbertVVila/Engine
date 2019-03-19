#ifndef __ComponentParticles_h__
#define __ComponentParticles_h__

#include "Component.h"
#include <string>

struct Texture;

class ComponentParticles :
	public Component
{
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

	int xTiles = 0;
	int yTiles = 0;
	float fps = 24.f;
};

#endif __ComponentParticles_h__