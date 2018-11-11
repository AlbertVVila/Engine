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

protected:
	bool enabled = true;
};

#endif //__Panel_h__