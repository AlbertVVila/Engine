#ifndef  __ProjectileScript_h__
#define  __ProjectileScript_h__

#include "BaseScript.h"

#ifdef ProjectileScript_EXPORTS
#define ProjectileScript_API __declspec(dllexport)
#else
#define ProjectileScript_API __declspec(dllimport)
#endif

class ProjectileScript_API ProjectileScript : public Script
{
};

#endif __ProjectileScript_h__
