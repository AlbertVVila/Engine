#ifndef  __MenuSoundsScript_h__
#define  __MenuSoundsScript_h__

#include "BaseScript.h"
#include <vector>

#ifdef MenuSoundsScript_EXPORTS
#define MenuSoundsScript_API __declspec(dllexport)
#else
#define MenuSoundsScript_API __declspec(dllimport)
#endif

class Button;
class ComponentAudioSource;

class MenuSoundsScript_API MenuSoundsScript : public Script
{
public:
	void Start() override;
	void Update() override;


public:

	std::vector<Component*> buttons;

	GameObject* Menu = nullptr;

	ComponentAudioSource* audioClick = nullptr;
	ComponentAudioSource* audioHovered = nullptr;
private:

	bool itemHovered = false;
	bool itemClicked = false;



};

#endif __MenuSoundsScript_h__
