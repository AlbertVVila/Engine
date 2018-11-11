#ifndef __PanelConfiguration_h__
#define __PanelConfiguration_h__

#include "Panel.h"
#include <vector>

class PanelConfiguration :
	public Panel
{
public:
	PanelConfiguration();
	~PanelConfiguration();

	void Draw();
	void AddFps(float fps);

private:
	void DrawFPSgraph() const;
	void DrawMemoryStats() const;

private:
	std::vector<float> fps = std::vector<float>();
};

#endif //__PanelConfiguration_h__