#ifndef __Skybox_h__
#define __Skybox_h__

#include "Geometry/Frustum.h"

class Shader;

class Skybox
{
public:
	Skybox();
	~Skybox();
	void Draw(const math::Frustum& frustum) const;

public:
	bool enabled = true;
	
private:
	Shader* shader = nullptr;
	unsigned skybox_cubemap = 0u;
	unsigned skyboxVAO = 0u; 
	unsigned skyboxVBO = 0u;
};

#endif __Skybox_h__
