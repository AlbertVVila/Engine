#ifndef __PanelResourceManager_h__
#define __PanelResourceManager_h__

#include "Panel.h"
#include <vector>

class Resource;
class ResourceTexture;
struct ImVec2;

enum class SORTING { NONE, UID, NAME, FILE, EXPORTED, TYPE, REFERENCES};
enum class RESOURCE_FILTER { TEXTURE, MODEL, MESH, AUDIO, SCENE, ANIMATION, MATERIAL, SKYBOX, STATEMACHINE, PREFAB, UNKNOWN, NONE};

class PanelResourceManager : 
	public Panel
{
public:
	PanelResourceManager();
	~PanelResourceManager();

	void Draw() override;

private:
	void UpdateResourcesList();
	void UpdateResourcesList(RESOURCE_FILTER filter);

	void OpenResourceEditor();

	void DrawResourceTexture();
	void DrawResourceModel();
	void DrawResourceMesh();
	void DrawResourceAudio();
	void DrawResourceScene();
	void DrawResourceMaterial();
	void DrawResourceAnimation();
	void DrawResourceSkybox();
	void DrawResourceSM();
	void DrawResourcePrefab();

	void DrawFilterByResourceMenu();

	void CleanUp();

private:
	Resource* auxResource = nullptr;
	Resource* previous = nullptr;

	int auxUID = 0;
	int auxReferences = 0;

	bool openEditor = false;
	bool openResourceWindow = false;

	SORTING sortList = SORTING::REFERENCES;
	RESOURCE_FILTER filterByResource = RESOURCE_FILTER::NONE;
	bool descending = false;
	std::vector<Resource*> resourcesList;

	bool autoRefresh = false;
};

#endif __PanelResourceManager_h__