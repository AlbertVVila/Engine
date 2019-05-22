#ifndef __PanelAnimation_h__
#define __PanelAnimation_h__

#include "Panel.h"

class PanelAnimation : public Panel
{
public:
	PanelAnimation();
	~PanelAnimation();

	void Draw() override;

	void UpdateGameObjectAnimation(GameObject* go, ResourceAnimation* anim);
	void CreateAnimationFromClip(ResourceAnimation* anim, int minFrame, int maxFrame);
	void NewEventPopUp(ComponentAnimation* compAnim);

public:
	int minFrame;
	int maxFrame;

	bool isCliping		 = false;
	bool newEvent		 = false;

	int keyToDelete = -1;
	bool isDeletingEvent = false;

	char newName[64] = "New Event";

	std::vector<std::string> guiAnimations;

	ResourceAnimation* anim = nullptr;
	//bool isPlaying = false;

};

#endif // !__PanelAnimation_h__