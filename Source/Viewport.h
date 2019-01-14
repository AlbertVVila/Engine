#ifndef __Viewport_h__
#define __Viewport_h__

#include "Math/float3.h"
class ComponentCamera;

class Viewport
{
public:
	Viewport(std::string name);
	~Viewport();

	void Draw(ComponentCamera* cam, bool isEditor = false);

	void CreateFrameBuffer(int width, int height);

	void CreateMSAABuffers(int width, int height);

	void DrawImGuizmo(const ComponentCamera & cam);

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
	bool focus = true;

private:
	bool enabled = true;
	std::string name = "Viewport";

	bool useSnap = false; //TODO: save in config
	float3 snapSettings = float3::one;
};

#endif __Viewport_h__