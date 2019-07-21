#ifndef __MOUSECONTROLLER_H_
#define __MOUSECONTROLLER_H_

#include "Globals.h"

#include <string>

class MouseController
{
public:
	MouseController();
	~MouseController();
	
	ENGINE_API void static ChangeCursorIcon(std::string);
};

#endif //  __MOUSECONTROLLER_H_