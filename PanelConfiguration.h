#ifndef __PanelConfiguration_h__
#define __PanelConfiguration_h__

#include "Panel.h"
#include <vector>

#define NUMFPS 100
class PanelConfiguration :
	public Panel
{
public:
	PanelConfiguration();
	~PanelConfiguration();

	void Draw();
	void DrawFPSgraph();
	void AddFps(float fps);

	void DrawMemoryStats() const;

private:
	std::vector<float> fps = std::vector<float>();
};

#endif //__PanelConfiguration_h__