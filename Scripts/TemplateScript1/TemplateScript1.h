#ifndef  __TemplateScript1_h__
#define  __TemplateScript1_h__

class GameObject;
struct ImGuiContext;
#include "BaseScript.h"

#ifdef TemplateScript1_EXPORTS
#define TemplateScript1_API __declspec(dllexport)
#else
#define TemplateScript1_API __declspec(dllimport)
#endif

class TemplateScript1_API TemplateScript1 : public Script
{
public:

protected:
	GameObject * gameObject;
};

extern "C" TemplateScript1_API TemplateScript1* CreateScript();

#endif __TemplateScript1_h__
