#ifndef __PanelResourceManager_h__
#define __PanelResourceManager_h__

#include "Panel.h"

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
};

#endif __PanelResourceManager_h__