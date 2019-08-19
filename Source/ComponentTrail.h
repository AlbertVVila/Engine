#ifndef __COMPONENT_TRAIL_H_
#define __COMPONENT_TRAIL_H_

#include "Component.h"
#include <queue>
#include <list>
#include "Math/float3.h"
#include "Math/float4.h"

class ParticleModule;


struct TrailPoint
{
	float remainingTime = .0f;
	float totalTime = .0f;
	math::float3 position = math::float3::zero;
	math::float3 rightPoint = math::float3::zero;
	float width = .0f;
	bool renderizable = false;

	TrailPoint(float totalTime, math::float3 position, float width) : remainingTime(totalTime), totalTime(totalTime), position(position), width(width) {}
	TrailPoint(float totalTime, math::float3 position, math::float3 previousPoint, float width, math::float3 up) : remainingTime(totalTime), totalTime(totalTime), position(position), width(width)
	{
		math::float3 cross = (previousPoint - position).Normalized().Cross(up);
		rightPoint = cross.Normalized();
		renderizable = true;
	}

	inline float Distance(math::float3 otherPoint)
	{
		return position.Distance(otherPoint);
	}

};

class ComponentTrail :
	public Component
{
	friend class ModuleParticles;

public:
		
	ComponentTrail(GameObject* gameobject);
	ComponentTrail(const ComponentTrail& component);

	~ComponentTrail();

	void UpdateTrail();
	void DrawProperties() override;
	void Save(JSON_value* value) const override;
	void Load(JSON_value* value) override;
	virtual void Enable(bool enable) override;

	ComponentTrail* Clone() const;
	std::queue<TrailPoint> trail;
	std::vector<ParticleModule*> modules;
	ResourceTexture* texture = nullptr;

private:

	float width = 100.f;
	float duration = .5f;
	float minDistance = 1.f;
	float bloomIntensity = 1.f;

	math::float4 trailColor = math::float4::one;

	std::string textureName = "None Selected";
	std::vector<std::string> textureFiles;
};



#endif
