#pragma once

#include "DebugUtils/DebugDraw.h"
#include "Math/float3.h"
#include "Math/float4.h"
#include <vector>


struct Shader;
struct v3_c4
{
	math::float3 position;
	//math::float4 color;
};
struct Shape
{
	unsigned drawMode = 0;
	unsigned start = 0;
	unsigned end = 0;
	float size = 0.0f;
};

class DetourDebugInterface : public duDebugDraw
{
public:
	DetourDebugInterface();
	~DetourDebugInterface();
	virtual unsigned int areaToCol(unsigned int area);
	virtual void depthMask(bool state);
	virtual void texture(bool state);
	virtual void begin(duDebugDrawPrimitives prim, float size = 1.0f);
	math::float4 getColor(unsigned color);
	virtual void vertex(const float* pos, unsigned int color);
	virtual void vertex(const float x, const float y, const float z, unsigned int color);
	virtual void vertex(const float* pos, unsigned int color, const float* uv);
	virtual void vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v);
	void debugDrawNavMesh();
	virtual void end();

private:
	std::vector<v3_c4> vertices;
	std::vector<Shape*> shapes;
	Shape* currentShape = nullptr;
	unsigned VAO = 0;
	unsigned VBO = 0;
	Shader* shader = nullptr;
};

