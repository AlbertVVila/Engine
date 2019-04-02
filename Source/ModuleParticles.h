#ifndef __ModuleParticles_h__
#define __ModuleParticles_h__

#include "Module.h"
#include <list>
#include "Math/float3.h"

class ComponentParticles;
class ComponentCamera;
class Shader;
class ComponentTrail;

#define MAX_TRAIL_INDICES 999 // 333 tris

class ModuleParticles :
	public Module
{
public:

	enum class ParticleSystemType
	{
		ANIMATION_STATIC
	};

	//ModuleParticles();
	~ModuleParticles();

	bool Start() override;
	bool CleanUp() override;

	void Render(float dt, const ComponentCamera* camera);

	void AddParticleSystem(ComponentParticles* cp);
	void AddTrailRenderer(ComponentTrail* cr);
	void RemoveTrailRenderer(ComponentTrail* cr);

private:
	
	void DrawAnimationStatic(ComponentParticles* cp, const ComponentCamera* camera) const;
	void RenderTrail(ComponentTrail* ct, const ComponentCamera* camera);

	std::list<ComponentParticles*> particleSystems;

	unsigned billBoardVAO = 0u;
	unsigned billBoardVBO = 0u;
	unsigned billBoardEBO = 0u;

	unsigned trailVAO = 0u;
	unsigned trailVBO = 0u;
	unsigned trailEBO = 0u;

	Shader* shader = nullptr;
	ParticleSystemType type = ParticleSystemType::ANIMATION_STATIC;

	std::list<ComponentTrail*> trails;

	math::float3 trailData[MAX_TRAIL_INDICES];
};

#endif __ModuleParticles_h__