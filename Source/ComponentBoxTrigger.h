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
	virtual void DrawProperties() override;

	virtual void Update() override;
	virtual void OnPlay() override;
	void DrawDebug();

	void SetIsPlayer(bool is_player);
	inline bool GetIsPlayer() const { return is_player; }

	inline const math::OBB* GetOBB() const { return box_trigger; }

	inline math::float3 GetBoxCenter() const { return position; }
	inline void SetBoxCenter(math::float3 center) { position = center; box_trigger->pos = center; }
	inline void SetBoxPosition(float x, float y, float z) { position = math::float3(x, y, z); box_trigger->pos = position; }

	inline math::float3 GetBoxSize() const { return size; }
	inline void SetBoxSize(math::float3 sizes) { size = sizes;  box_trigger->r = sizes; }
	inline void SetBoxSize(float width, float height, float depth) { size = math::float3(width, height, depth); box_trigger->r = size; }

	void AddOverlap(const ComponentBoxTrigger* other);
	void RemoveOverlap(const ComponentBoxTrigger* other);

	void Save(JSON_value* value) const;
	void Load(JSON_value* value);

private:
	void PropagateState(GameObject* other, Overlap_State state);

private:
	std::unordered_map<const ComponentBoxTrigger*,Overlap_State> overlap_list;
	math::OBB* box_trigger = nullptr;

	math::float3 position = math::float3::zero;
	math::float3 size     = math::float3::one;

	bool is_player = false;
	bool debug_draw = false;
};

#endif // !__Component_BoxTrigger_H__

