#ifndef __PanelScene_h__
#define __PanelScene_h__

#include "Panel.h"
class PanelScene :
	public Panel
{
public:
	PanelScene();
	~PanelScene();

	void Draw();

	void CreateFrameBuffer(int width, int height);

	void CreateMSAABuffers(int width, int height);

	void DrawImGuizmo();

	void Pick();

public:
	int current_width = 0;
	int current_height = 0;

	unsigned texture = 0; //Handled allocation inside class
	unsigned FBO = 0;
	unsigned RBO = 0;

	unsigned MSAAFBO = 0;
	unsigned MSAADEPTH = 0;
	unsigned MSAACOLOR = 0;
private:
};

#endif //__PanelScene_h__

