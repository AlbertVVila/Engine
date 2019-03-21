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
	std::string path;
	std::string fileSelected;
	std::stack<std::string> pathStack;

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

	// ImGui elements size
	math::float2 fileSize = math::float2(40.0f, 40.0f);
};

#endif //__PanelBrowser_h__