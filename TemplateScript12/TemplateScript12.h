#ifndef  __TemplateScript12_h__
#define  __TemplateScript12_h__

#include "BaseScript.h"

#ifdef TemplateScript12_EXPORTS
#define TemplateScript12_API __declspec(dllexport)
#else
#define TemplateScript12_API __declspec(dllimport)
#endif

class TemplateScript12_API TemplateScript12 : public Script
{
public:
	inline virtual TemplateScript12* Clone() const
	{
		return new TemplateScript12(*this);
	}
};

#endif __TemplateScript12_h__
