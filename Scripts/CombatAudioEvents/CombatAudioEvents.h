#ifndef  __CombatAudioEvents_h__
#define  __CombatAudioEvents_h__

#include "BaseScript.h"

#ifdef CombatAudioEvents_EXPORTS
#define CombatAudioEvents_API __declspec(dllexport)
#else
#define CombatAudioEvents_API __declspec(dllimport)
#endif


class ComponentAudioSource;

class CombatAudioEvents_API CombatAudioEvents : public Script
{
public:
	ComponentAudioSource* m1AttackSound = nullptr;

public:
	void Start() override;
	void OnAnimationEvent(std::string name) override;

};

#endif __CombatAudioEvents_h__
