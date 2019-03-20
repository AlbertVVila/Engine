#ifndef __ModuleParticles_h__
#define __ModuleParticles_h__

#include "Module.h"
#include <list>

class ComponentParticles;
class ComponentCamera;
class Shader;

class ModuleParticles :
	public Module
{
public:

	enum class ParticleSystemType
	{
		ANIMATION_STATIC
	};

	//ModuleParticles();
	//~ModuleParticles();

	bool Start() override;
	bool CleanUp() override;

	void Render(float dt, const ComponentCamera* camera);

	void AddParticleSystem(ComponentParticles* cp);


private:
	
	void DrawAnimationStatic(ComponentParticles * cp, const ComponentCamera * camera) const;
	
	std::list<ComponentParticles*> particleSystems;

	unsigned VAO = 0u;
	unsigned VBO = 0u;
	unsigned EBO = 0u;
	Shader* shader = nullptr;
	ParticleSystemType type = ParticleSystemType::ANIMATION_STATIC;

};

#endif __ModuleParticles_h__