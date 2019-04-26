#ifndef __MODULENAVIGATION_H__
#define __MODULENAVIGATION_H__

#include "Module.h"

#include "Recast/Recast.h"
#include "debugdraw.h"

#include <vector>

//#include <stdio.h>
#include <cstddef>

typedef struct _WOWPOS 
{ 
	float x;
	float y;
	float z;}

WOWPOS;

#define MAX_PATH 1024

//#include "DebugUtils/DetourDebugDraw.h"
//#include "DebugUtils/DebugDraw.h"
//<>
//fwd declarations
class GameObject;
class ComponentRenderer;
class ComponentTransform;
//from recast
class rcConfig;
class rcContext;
class rcCompactHeightfield;
class rcHeightfield;
class rcContourSet;
class rcPolyMesh;
class rcPolyMeshDetail;
//class DrawVertex;
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

class ModuleNavigation :
	public Module
{
public:
	ModuleNavigation();
	~ModuleNavigation();

	void DrawGUI()override;
	void navigableObjectToggled(GameObject* obj, const bool newState);
	
	void renderNavMesh();
	void cleanValues();

	bool FindPath(math::float3 start, math::float3 end, std::vector<math::float3> &path) const;
	void RecalcPath(math::float3 point);

	//variables
	std::vector<GameObject*> navigationMeshes;
	std::vector<GameObject*> agents;
	std::vector<GameObject*> obstacles;

	//Constants
	//static const int ERROR = -1;
	static const int ERROR_NEARESTPOLY = -2;

private:
	// Explicitly-disabled copy constructor and copy assignment operator.
	ModuleNavigation(const ModuleNavigation&);
	ModuleNavigation& operator=(const ModuleNavigation);

	void removeNavMesh(unsigned ID);
	void generateNavigability();
	void addNavigableMesh();

	void fillVertices();
	void fillIndices();
	void fillNormals();
	void fillDrawPoints();

	void cleanUpNavValues();

	// Detour stuff
	std::vector<math::float3> returnPath(math::float3 pStart, math::float3 pEnd);
	//void handleClick(const float* s, const float* p, bool shift);
	//std::vector<math::float3> returnPath(math::float3 pStart, math::float3 pEnd);
	//void handleClick(const float* s, const float* p, bool shift);
	//int FindStraightPath(WOWPOS start, WOWPOS end, WOWPOS* path, int size);
	
private:
	//variables
	float maxRadius = 5.0f;
	float maxHeight = 5.0f;
	float maxSlopeScaling = 20.0f;
	float maxStepHeightScaling = 5.0f;
	
	char newCharacter[64] = "New Character";
	float characterMaxRadius = 0.6f;
	float characterMaxHeight = 5.0f;//might need higher val
	float characterMaxSlopeScaling = 50.0f;
	float characterMaxStepHeightScaling = 5.0f;//might need higher value
	
	const float sliderIncreaseSpeed = 0.03f;
	const float minSliderValue = 0.01f;
	const float maxSliderValue = 100.0f;
	const float maxSlopeValue = 60.0f;
	const float cellIncreaseSpeed = 0.25f;
	const float minCellSize = 0.1f;
	const float maxCellSize = 10.0f;
	int minRegionSize = 8;
	int mergedRegionSize = 20;
	float edgeMaxLength = 20.f;
	float edgeMaxError = 1.3f;
	int vertexPerPoly = 6;

	//newer config
	float cellWidth = 1.f;
	float cellHeight = 5.f;
	
	float sampleDistance = 6;
	float sampleMaxError = 1;

	//filters
	bool filterLowHangingObstacles = true;
	bool filterLedgeSpans = true;
	bool filterWalkableLowHeightSpans = true;

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

	std::vector < const ComponentRenderer*> meshComponents;
	std::vector < const ComponentTransform*> transformComponents;

	rcConfig cfg;
	rcContext* ctx = nullptr;
	rcCompactHeightfield* chf = nullptr;
	rcHeightfield* heightField = nullptr;
	rcContourSet* cset = nullptr;
	rcPolyMesh* pmesh = nullptr;
	rcPolyMeshDetail* dmesh = nullptr;

	dtNavMesh* navMesh = nullptr;
	dtNavMeshQuery* navQuery = nullptr;

	//SampleDebugDraw dt_dd;

	unsigned char* m_triareas = nullptr;
	bool m_keepInterResults = true;

	int* tris = nullptr;
	float* verts = nullptr;
	float* normals = nullptr;
	int nverts = 0;
	int ntris = 0;
	bool pointsUpdated = false;
	dd::DrawVertex* points = nullptr;
	std::vector<const AABB*> meshboxes;
	float* bmin = nullptr;
	float* bmax = nullptr;
	const AABB* meshbox = nullptr;

	//Detour pathfinding

	bool pathGenerated = false;
	std::vector<math::float3> path;
	math::float3 start = math::float3::inf;
	math::float3 end = math::float3::inf;

	bool startPoint = true; //defines if we are going to select start or end point in debug mode

};

#endif
