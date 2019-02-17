#ifndef  __PlayerMovement_h__
#define  __PlayerMovement_h__

class GameObject;
struct ImGuiContext;
#include "BaseScript.h"

#ifdef PlayerMovement_EXPORTS
#define PlayerMovement_API __declspec(dllexport)

#else
#define PlayerMovement_API __declspec(dllimport)
#endif

class PlayerMovement_API PlayerMovement : public Script
{
public:
	void Expose(ImGuiContext *context) override;
	void Update() override;
	//void Serialize(JSON* json) const override;
	//void DeSerialize(JSON * json) override;

private:
	float speed = 2.0f;
};

extern "C" PlayerMovement_API PlayerMovement* CreateScript();

#endif __PlayerMovement_h__
