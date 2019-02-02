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

protected:
	GameObject * gameObject;
};

extern "C" PlayerMovement_API PlayerMovement* CreateScript();

#endif __PlayerMovement_h__
