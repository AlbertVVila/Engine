#ifndef  __CreditsScript_h__
#define  __CreditsScript_h__

#include "BaseScript.h"

#ifdef CreditsScript_EXPORTS
#define CreditsScript_API __declspec(dllexport)
#else
#define CreditsScript_API __declspec(dllimport)
#endif

class ComponentTransform2D;

class CreditsScript_API CreditsScript : public Script
{
public:
	void Expose(ImGuiContext* context) override;

	void Start() override;
	void Update() override;

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;
public:
	bool creditsDone = false;

private:
	ComponentTransform2D* transform2D = nullptr;
	float speed = 100.0f;
	float stopValue = 900.0f;
};

extern "C" CreditsScript_API CreditsScript* CreateScript();

#endif __CreditsScript_h__
