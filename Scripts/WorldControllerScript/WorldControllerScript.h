#ifndef  __WorldControllerScript_h__
#define  __WorldControllerScript_h__

#include "BaseScript.h"

#ifdef WorldControllerScript_EXPORTS
#define WorldControllerScript_API __declspec(dllexport)
#else
#define WorldControllerScript_API __declspec(dllimport)
#endif

class WorldControllerScript_API WorldControllerScript : public Script
{
};

#endif __WorldControllerScript_h__
