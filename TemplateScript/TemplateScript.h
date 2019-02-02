#ifndef  __TemplateScript_h__
#define  __TemplateScript_h__

class GameObject;
struct ImGuiContext;
#include "BaseScript.h"

#ifdef TEMPLATESCRIPT_EXPORTS
#define TEMPLATESCRIPT_API __declspec(dllexport)

#else
#define TEMPLATESCRIPT_API __declspec(dllimport)
#endif

class TEMPLATESCRIPT_API TemplateScript : public Script
{
public:

protected:
	GameObject * gameObject;
};

extern "C" TEMPLATESCRIPT_API TemplateScript* CreateScript();

#endif __TemplateScript_h__