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
	void AddFps(float fps); //TODO ms graph

private:
	void DrawFPSgraph(); //updates min framerate
	void DrawMemoryStats() const;

private:
	std::vector<float> fps = std::vector<float>();
	float fmin = 2000;
};

#endif //__PanelConfiguration_h__