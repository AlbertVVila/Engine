#ifndef __MODULENAVIGATION_H__
#define __MODULENAVIGATION_H__

#include "Module.h"
#include <vector>
//<>
class GameObject;
class ComponentRenderer;

struct navConfig {
	int width;
	int height;
	float bmin[3];
	float bmax[3];
	float cs;
	float ch;
	float walkableSlopeAngle;
	int walkableHeight;
	int walkableClimb;
	int walkableRadius;
	int maxEdgeLen;
	float maxSimplificationError;
	int minRegionArea;		// Note: area = size*size
	int mergeRegionArea;	// Note: area = size*size
	int maxVertsPerPoly;
	float detailSampleDist;
	float detailSampleMaxError;
	
};

class ModuleNavigation :
	public Module
{
public:
	ModuleNavigation();
	~ModuleNavigation();

	void DrawGUI()override;
	void navigableObjectToggled(GameObject* obj, const bool newState);

	//variables
	std::vector<GameObject*> navigationMeshes;
	std::vector<GameObject*> agents;
	std::vector<GameObject*> obstacles;

private:
	void removeNavMesh(unsigned ID);
	void generateNavigability();

	void fillVertices(float* verts, const int nverts);

	void fillIndices(int* tris, const int ntris);

	void cleanUpNavValues();

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

	//newer config
	float cellWidth = 10.f;
	float cellHeight = 10.f;

	//navigation mesh properties
	const ComponentRenderer* meshComponent = nullptr;

	navConfig cfg;

	int* tris = nullptr;
	float* verts = nullptr;
};

#endif