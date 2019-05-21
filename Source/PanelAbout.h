#ifndef __PanelAbout_h__
#define __PanelAbout_h__

#include "Panel.h"
#include "ft2build.h"
#include "freeType/freetype.h"
#include <string>

class PanelAbout : public Panel
{
public:
	PanelAbout();
	~PanelAbout();

	void Draw();

public:
	std::string toChar = "FreeType (" + std::to_string(FREETYPE_MAJOR)+ "." + std::to_string(FREETYPE_MINOR) + "." + std::to_string(FREETYPE_PATCH) + ")";
	const char * freeTypeVersion = toChar.c_str();
};

#endif //__PanelAbout_h__