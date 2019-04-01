#ifndef __PanelAnimation_h__
#define __PanelAnimation_h__

#include "Panel.h"

class PanelAnimation : public Panel
{
public:
	PanelAnimation();
	~PanelAnimation();

	void Draw() override;

	void UpdateGameObjectAnimation(GameObject* go, Animation* anim);

public:
	int minFrame;
	bool isCliping = false;

	//bool isPlaying = false;

};

#endif // !__PanelAnimation_h__