#include "TemplateScript.h"

TEMPLATESCRIPT_API TemplateScript* CreateScript()
{
	TemplateScript* instance = new TemplateScript;
	return instance;
}

