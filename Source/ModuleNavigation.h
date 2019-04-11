#ifndef __MODULENAVIGATION_H__
#define __MODULENAVIGATION_H__

#include "Module.h"
#include <vector>
#include "Recast/Recast.h"
#include "DebugUtils/DetourDebugDraw.h"
#include "DebugUtils/DebugDraw.h"
//<>
//fwd declarations
class GameObject;
class ComponentRenderer;
//from recast
class rcConfig;
class rcContext;
class rcCompactHeightfield;
class rcHeightfield;
class rcContourSet;
class rcPolyMesh;
class rcPolyMeshDetail;
//from detour
class dtNavMesh;
class dtNavMeshQuery;

/// These are just sample areas to use consistent values across the samples.
/// The use should specify these base on his needs.
enum SamplePolyAreas
{
	SAMPLE_POLYAREA_GROUND,
	SAMPLE_POLYAREA_WATER,
	SAMPLE_POLYAREA_ROAD,
	SAMPLE_POLYAREA_DOOR,
	SAMPLE_POLYAREA_GRASS,
	SAMPLE_POLYAREA_JUMP,
};
enum SamplePolyFlags
{
	SAMPLE_POLYFLAGS_WALK = 0x01,		// Ability to walk (ground, grass, road)
	SAMPLE_POLYFLAGS_SWIM = 0x02,		// Ability to swim (water).
	SAMPLE_POLYFLAGS_DOOR = 0x04,		// Ability to move through doors.
	SAMPLE_POLYFLAGS_JUMP = 0x08,		// Ability to jump.
	SAMPLE_POLYFLAGS_DISABLED = 0x10,		// Disabled polygon
	SAMPLE_POLYFLAGS_ALL = 0xffff	// All abilities.
};

enum SamplePartitionType
{
	SAMPLE_PARTITION_WATERSHED,
	SAMPLE_PARTITION_MONOTONE,
	SAMPLE_PARTITION_LAYERS,
};

/// OpenGL debug draw implementation.
class DebugDrawGL : public duDebugDraw
{
public:
	virtual void depthMask(bool state);
	virtual void texture(bool state);
	virtual void begin(duDebugDrawPrimitives prim, float size = 1.0f);
	virtual void vertex(const float* pos, unsigned int color);
	virtual void vertex(const float x, const float y, const float z, unsigned int color);
	virtual void vertex(const float* pos, unsigned int color, const float* uv);
	virtual void vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v);
	virtual void end();
};

class SampleDebugDraw : public DebugDrawGL
{
public:
	virtual unsigned int areaToCol(unsigned int area);
};

class ModuleNavigation :
	public Module
{
public:
	ModuleNavigation();
	~ModuleNavigation();

	void DrawGUI()override;
	void navigableObjectToggled(GameObject* obj, const bool newState);
	
	void renderNavMesh();
	

	//variables
	std::vector<GameObject*> navigationMeshes;
	std::vector<GameObject*> agents;
	std::vector<GameObject*> obstacles;

private:
	// Explicitly-disabled copy constructor and copy assignment operator.
	ModuleNavigation(const ModuleNavigation&);
	ModuleNavigation& operator=(const ModuleNavigation);

	void removeNavMesh(unsigned ID);
	void generateNavigability();

	void fillVertices();
	void fillIndices();
	void fillNormals();

	void cleanUpNavValues();

	void drawConvexVolumes(struct duDebugDraw* dd, bool hilight = false);

	//variables
	float maxRadius = 5.0f;
	float maxHeight = 5.0f;
	float maxSlopeScaling = 20.0f;
	float maxStepHeightScaling = 5.0f;
	
	char newCharacter[64] = "New Character";
	float characterMaxRadius = 5.0f;
	float characterMaxHeight = 5.0f;
	float characterMaxSlopeScaling = 20.0f;
	float characterMaxStepHeightScaling = 5.0f;
	
	const float sliderIncreaseSpeed = 0.03f;
	const float minSliderValue = 0.01f;
	const float maxSliderValue = 100.0f;
	const float maxSlopeValue = 60.0f;
	const float cellIncreaseSpeed = 5.0f;
	const float minCellSize = 10.0f;
	const float maxCellSize = 100.0f;

	//newer config
	float cellWidth = 20.f;
	float cellHeight = 10.f;
	float edgeMaxLength = 30.f;
	float edgeMaxError = 5.f;
	int minRegionSize = 10;
	int mergedRegionSize = 20;
	int vertexPerPoly = 10;
	float sampleDistance = 10;
	float sampleMaxError = 2;

	//filters
	bool filterLowHangingObstacles;
	bool filterLedgeSpans;
	bool filterWalkableLowHeightSpans;

	//partition type
	int partitionType = 0;

	//navigation mesh properties
	bool meshGenerated = false;

	enum DrawMode
	{
		DRAWMODE_NAVMESH,
		DRAWMODE_NAVMESH_TRANS,
		DRAWMODE_NAVMESH_BVTREE,
		DRAWMODE_NAVMESH_NODES,
		DRAWMODE_NAVMESH_INVIS,
		DRAWMODE_MESH,
		DRAWMODE_VOXELS,
		DRAWMODE_VOXELS_WALKABLE,
		DRAWMODE_COMPACT,
		DRAWMODE_COMPACT_DISTANCE,
		DRAWMODE_COMPACT_REGIONS,
		DRAWMODE_REGION_CONNECTIONS,
		DRAWMODE_RAW_CONTOURS,
		DRAWMODE_BOTH_CONTOURS,
		DRAWMODE_CONTOURS,
		DRAWMODE_POLYMESH,
		DRAWMODE_POLYMESH_DETAIL,
		MAX_DRAWMODE
	};

	DrawMode m_drawMode;

	const ComponentRenderer* meshComponent = nullptr;

	rcConfig cfg;
	rcContext* ctx;
	rcCompactHeightfield* chf;
	rcHeightfield* heightField;
	rcContourSet* cset;
	rcPolyMesh* pmesh;
	rcPolyMeshDetail* dmesh;

	dtNavMesh* navMesh;
	dtNavMeshQuery* navQuery;

	unsigned char m_navMeshDrawFlags = DU_DRAWNAVMESH_OFFMESHCONS | DU_DRAWNAVMESH_CLOSEDLIST;
	SampleDebugDraw dd;

	unsigned char* m_triareas = nullptr;
	bool m_keepInterResults = true;

	int* tris = nullptr;
	float* verts = nullptr;
	float* normals = nullptr;
	int nverts;
	int ntris;
	const AABB* meshbox = nullptr;
};

#endif