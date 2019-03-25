#ifndef __PanelBrowser_h__
#define __PanelBrowser_h__

#include "Panel.h"
#include "ResourceTexture.h"

#include "MathGeoLib\include\Math\float2.h"
#include <string>
#include <vector>
#include <stack>

class PanelBrowser :
	public Panel
{
public:
	PanelBrowser();
	~PanelBrowser();

	bool Init() override;
	void Draw();

private:
	void DrawFolderIcon(const char* dir, int itemNumber);
	void DrawFileIcon(const char* file, int itemNumber);

	void DrawFileContextMenu();
	void DrawImportConfigurationPopUp();
	void DrawRenamePopUp();

public:
	bool folderContentDirty = true;

private:
	std::string path;
	//std::string fileSelected;
	std::stack<std::string> pathStack;
	std::vector<std::string> files;		// List of the files in current path
	std::vector<std::string> dirs;		// List of the folders in current path

	bool openImportConfigPopUp = false;
	bool openRenamePopUp = false;
	Resource* fileSelected = nullptr;

	std::string newName;				// Aux string for rename

	// Icons
	ResourceTexture* folderIcon;	
	ResourceTexture* fileIcon;
	ResourceTexture* fbxIcon;
	ResourceTexture* pngIcon;
	ResourceTexture* jpgIcon;
	ResourceTexture* tgaIcon;
	ResourceTexture* tifIcon;
	ResourceTexture* ddsIcon;
	ResourceTexture* m4tIcon;
	ResourceTexture* jsonIcon;
};

#endif //__PanelBrowser_h__