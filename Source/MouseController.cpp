#include "MouseController.h"

#define OEMRESOURCE
#include <windows.h>
#include <assert.h>

MouseController::MouseController()
{
}

MouseController::~MouseController()
{
}

void MouseController::ChangeCursorIcon(std::string cursorPath)
{
	HCURSOR handCursor = LoadCursorFromFile(cursorPath.c_str());
	BOOL ret = SetSystemCursor(handCursor, OCR_NORMAL);
	assert(ret);
	DestroyCursor(handCursor);
}

