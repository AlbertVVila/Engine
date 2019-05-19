#ifndef __ComponentParticles_h__
#define __ComponentParticles_h__

#include "Component.h"
#include <string>
#include "Math/float3.h"
#include "Math/float2.h"
#include "Math/float4x4.h"
#include "Math/Quat.h"
#include "Math/float4.h"

#include <queue>
#include <vector>
#include "ComponentTransform.h"
#include "ModuleRender.h"


class ResourceTexture;
class ParticleModule;

struct Particle
{
	float speed = 1.f;

	math::float3 position = math::float3::zero;
	math::float4x4 global = math::float4x4::zero;
	math::float3 direction = math::float3::unitY;
	float size = 1.f;

	float totalLifetime = .0f;
	float lifeTimer = totalLifetime;
	math::float4 color;
	
};


class ComponentParticles :
	public Component
{
	friend class ModuleParticles;

public:

	enum EmisorType
	{
		QUAD = 0,
		SPHERE
	};

	ComponentParticles(GameObject* gameobject);
	ComponentParticles(const ComponentParticles& component);
	~ComponentParticles();


	Component* Clone() const override;
	void DrawProperties() override;
	bool CleanUp() override;

	void Update(float dt, const math::float3& camPos);

	void Save(JSON_value* value) const override;
	void Load(JSON_value* value) override;

	ResourceTexture* texture = nullptr;

	std::list<Particle*> particles;
	std::queue<Particle*> particlePool;

private:

	void alternateEmisor(int i);
	void DrawDebugEmisor();
	float3 randomSpherePoint(float3 center);

private:

	std::vector<std::string> textureFiles;

	std::vector<ParticleModule*> modules;

	int xTiles = 1;
	int yTiles = 1;
	float fps = 24.f;

	float timer = 0.f;
	float frameMix = 0.f;
	int f1Xpos;
	int f1Ypos;
	int f2Xpos;
	int f2Ypos;

	math::float2 lifetime = math::float2::one;
	math::float2 speed = math::float2::one * App->renderer->current_scale;
	float rate = 10.f;
	float rateTimer = 1.f / rate;
	int maxParticles = 50;
	math::float2 particleSize = math::float2(1.f * App->renderer->current_scale, 1.f * App->renderer->current_scale) ;
	float quadEmitterSize = 10.f * App->renderer->current_scale;
	float sphereEmitterRadius = 5.f * App->renderer->current_scale;
	math::float4 particleColor = math::float4::one;
	math::float3 pDir = math::float3(-1.f, 0.f, 0.f);

	bool directionNoise = false;
	int directionNoiseProbability = 5;
	int directionNoiseTotalProbability = 500;

	EmisorType actualEmisor = EmisorType::QUAD;
	bool quadCheck = true;
	bool sphereCheck = false;
	std::vector<bool*> emisorsCheck = { &quadCheck,&sphereCheck};

	bool sizeOTCheck = false;
	bool colorOTCheck = false;



};


#endif __ComponentParticles_h__