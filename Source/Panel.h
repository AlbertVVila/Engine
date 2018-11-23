#ifndef __Panel_h__
#define __Panel_h__
#include "ImGui\imgui.h"
class Panel
{
public:
	Panel() {};

	virtual ~Panel() {};

	virtual void Draw() {}

	bool IsEnabled() const
	{
		return enabled;
	}

	void ToggleEnabled()
	{
		enabled = !enabled;
	}

	void SetEnabled() 
	{ 
		enabled = true; 
	}

	bool IsFocused() const
	{
		return focus;
	}

protected:
	bool enabled = true;
	bool focus = false;
};

#endif //__Panel_h__