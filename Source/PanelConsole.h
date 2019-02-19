#ifndef __PanelConsole_h__
#define __PanelConsole_h__

#include "Panel.h"
#include <vector>
struct ImGuiTextBuffer;

class PanelConsole :
	public Panel
{
public:
	PanelConsole();
	~PanelConsole();

	void Clear();
	void AddLog(const char * log);
	void Draw();

private:
	bool ForceScrollToBottom = true;
	ImGuiTextBuffer* Buf = nullptr;
	std::vector<int> LineOffsets;        // Index to lines offset
	bool ScrollToBottom = false;
};

#endif //__PanelConsole_h__
