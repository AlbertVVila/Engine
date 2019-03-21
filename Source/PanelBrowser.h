#ifndef __PanelBrowser_h__
#define __PanelBrowser_h__

#include "Panel.h"
#include "ResourceTexture.h"
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
	ResourceTexture* FBXIcon;
};

#endif //__PanelBrowser_h__