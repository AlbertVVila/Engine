#ifndef __MODULENAVIGATION_H__
#define __MODULENAVIGATION_H__

#include "Module.h"
#include <vector>
//<>
class GameObject;
class ComponentRenderer;

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
	// Explicitly-disabled copy constructor and copy assignment operator.
	ModuleNavigation(const ModuleNavigation&);
	ModuleNavigation& operator=(const ModuleNavigation);

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
	float edgeMaxLength = 30.f;
	float edgeMaxError = 5.f;
	int minRegionSize = 10;
	int mergedRegionSize = 20;
	int vertexPerPoly = 10;
	float sampleDistance = 10;
	float sampleMaxError = 2;

	//navigation mesh properties
	const ComponentRenderer* meshComponent = nullptr;

	rcConfig cfg;
	rcContext ctx;
	rcHeightfield* heightField;

	unsigned char* m_triareas;
	bool m_keepInterResults = true;

	int* tris = nullptr;
	float* verts = nullptr;
};

#endif