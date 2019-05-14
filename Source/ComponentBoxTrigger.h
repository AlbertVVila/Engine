#ifndef __Component_BoxTrigger_H__
#define __Component_BoxTrigger_H__

#include "Component.h"
#include <unordered_map>
#include "Geometry/OBB.h"
#include "Math/float3.h"

enum class Overlap_State
{
	Enter = 0,
	Idle,
	PostIdle,
	Exit
};

class ComponentBoxTrigger : public Component
{
public:
	ComponentBoxTrigger();
	ComponentBoxTrigger(GameObject* gameobject);
	ComponentBoxTrigger(const ComponentBoxTrigger& component);
	virtual ~ComponentBoxTrigger();

	virtual Component* Clone() const override;
	virtual void DrawProperties() override {/*TODO*/};

	virtual void Update() override;

	void SetIsPlayer(bool is_player);
	inline bool GetIsPlayer() const { return is_player; }

	inline const math::OBB* GetOBB() const { return box_trigger; }

	inline math::float3 GetBoxCenter() const { return box_trigger->pos; }
	inline void SetBoxCenter(math::float3 center) { box_trigger->pos = center; }
	inline void SetBoxPosition(float x, float y, float z) { box_trigger->pos = math::float3(x, y, z); }

	inline math::float3 GetBoxSize() const { return box_trigger->r; }
	inline void SetBoxSize(math::float3 sizes) { box_trigger->r = sizes; }
	inline void SetBoxSize(float width, float height, float depth) { box_trigger->r = math::float3(width, height, depth); }

	void AddOverlap(const ComponentBoxTrigger* other);
	void RemoveOverlap(const ComponentBoxTrigger* other);

private:
	std::unordered_map<const ComponentBoxTrigger*,Overlap_State> overlap_list;
	math::OBB* box_trigger = nullptr;

	bool is_player = false;
};

#endif // !__Component_BoxTrigger_H__

