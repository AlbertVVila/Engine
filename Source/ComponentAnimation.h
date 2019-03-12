#ifndef __COMPONENTANIMATION_H_
#define __COMPONENTANIMATION_H_

#include "Component.h"
#include "Animation.h"

#include "Math/float4x4.h"
#include "Math/float3.h"
#include "Math/Quat.h"

struct Frame;

class ComponentAnimation : public Component
{
public:

	void DrawProperties();

	Component* Clone() const;
	void Update(Frame* frame, Channel* channel);

	ComponentAnimation();
	ComponentAnimation(GameObject* gameobject);
	ComponentAnimation(const ComponentAnimation &copy);
	~ComponentAnimation();

	void OffsetChannels(GameObject* GO);

	void Save(JSON_value* value) const override;
	void Load(JSON_value* value) override;

	Frame* InterpolateFrame(const Frame* first, const Frame* second, float lambda) const;
	math::float4x4 InterpolateFloat4x4(const math::float4x4& first, const math::float4x4& second, float lambda) const;
	math::float3 InterpolateFloat3(const math::float3& first, const math::float3& second, float lambda) const;
	math::Quat InterpolateQuat(const math::Quat& first, const math::Quat& second, float lambda) const;

	bool CleanUp();
public:

	Animation* anim = nullptr;
};

#endif //  __COMPONENTANIMATION_H_
