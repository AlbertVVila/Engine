#ifndef __PanelResourceManager_h__
#define __PanelResourceManager_h__

#include "Panel.h"
#include <vector>

class Resource;
class ResourceTexture;
struct ImVec2;

class PanelResourceManager : 
	public Panel
{
public:
	PanelResourceManager();
	~PanelResourceManager();

	void Draw() override;
	void UpdateResourcesList();

private:
	void OpenResourceEditor();
	void DrawResourceTexture();
	void DrawResourceMesh();
	void CleanUp();

private:
	Resource* auxResource = nullptr;
	Resource* previous = nullptr;

	int auxReferences = 0u;

	bool openEditor = false;
	bool openTextureWindow = false;
	bool openMeshWindow = false;

	std::vector<Resource*> resourcesList;
};

#endif __PanelResourceManager_h__