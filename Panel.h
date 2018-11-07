#ifndef __Panel_h__
#define __Panel_h__

#include "ImGui\imgui.h"
class Panel
{
public:
	Panel();
	~Panel();

	virtual void Draw() {}

	bool IsEnabled() const
	{
		return enabled;
	}

	void ToggleEnabled()
	{
		enabled = !enabled;
	}
protected:
	bool enabled = true;
};

#endif __Panel_h__