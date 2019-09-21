#ifndef  __DissolveScript_h__
#define  __DissolveScript_h__

#include "BaseScript.h"

class ComponentRenderer;

#ifdef DissolveScript_EXPORTS
#define DissolveScript_API __declspec(dllexport)
#else
#define DissolveScript_API __declspec(dllimport)
#endif

class DissolveScript_API DissolveScript : public Script
{
public:
	void Start() override;
	void Update() override;
	void OnTriggerEnter(GameObject* go) override;
	void Expose(ImGuiContext* context) override;

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

	inline virtual DissolveScript* Clone() const
	{
		return new DissolveScript(*this);
	}

private:
	ComponentRenderer* myRender = nullptr;

	float initialDissolveAmount = 0.0f;
	float finalDissolveAmount = 0.0f;
	float dissolveSpeed = 0.0f;
};

#endif __DissolveScript_h__
