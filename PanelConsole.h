#ifndef __PanelConsole_h__
#define __PanelConsole_h__

#include "Panel.h"
class PanelConsole :
	public Panel
{
public:
	PanelConsole();
	~PanelConsole();

	void Clear() { Buf.clear(); LineOffsets.clear(); }
	void AddLog(const char * log);
	void Draw();
private:
	ImGuiTextBuffer     Buf;
	ImVector<int>       LineOffsets;        // Index to lines offset
	bool                ScrollToBottom = false;
};

#endif //__PanelConsole_h__
