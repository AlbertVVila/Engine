#ifndef  __TestScript_h__
#define  __TestScript_h__

#include "BaseScript.h"

#ifdef TestScript_EXPORTS
#define TestScript_API __declspec(dllexport)
#else
#define TestScript_API __declspec(dllimport)
#endif

class TestScript_API TestScript : public Script
{
};

#endif __TestScript_h__
