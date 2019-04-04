#ifndef __COMPONENT_TRAIL_H_
#define __COMPONENT_TRAIL_H_

#include "Component.h"
#include <queue>
#include "Math/float3.h"

class ComponentTrail :
	public Component
{
public:
	
	struct TrailPoint
	{
		float remainingTime = .0f;
		math::float3 position = math::float3::zero;
		math::float3 leftPoint = math::float3::zero;
		math::float3 rightPoint = math::float3::zero;
		bool renderizable = false;

		TrailPoint(float remainingTime, math::float3 position) : remainingTime(remainingTime), position(position) {}
		TrailPoint(float remainingTime, math::float3 position, math::float3 previousPoint, float width, math::float3 up) : remainingTime(remainingTime), position(position)
		{
			math::float3 cross = (previousPoint - position).Normalized().Cross(up);
			rightPoint = position + cross.Normalized() * width;
			leftPoint = position - cross.Normalized() * width;
			renderizable = true;
		}
		
		float Distance(math::float3 otherPoint)
		{
			return position.Distance(otherPoint);
		}

	};

	ComponentTrail(GameObject* gameobject);
	ComponentTrail(const ComponentTrail& component);

	~ComponentTrail();

	void Update() override;
	void DrawProperties() override;
	void Save(JSON_value* value) const override;
	void Load(JSON_value* value) override;

	ComponentTrail* Clone() const;
	std::queue<TrailPoint> trail;

	Texture* texture = nullptr;

private:

	float width = 100.f;
	float duration = .5f;
	float minDistance = 1.f;

	std::string textureName = "None Selected";
	std::vector<std::string> textureFiles;
};

#endif
