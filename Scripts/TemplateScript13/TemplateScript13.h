#ifndef  __TemplateScript13_h__
#define  __TemplateScript13_h__

#include "BaseScript.h"

#ifdef TemplateScript13_EXPORTS
#define TemplateScript13_API __declspec(dllexport)
#else
#define TemplateScript13_API __declspec(dllimport)
#endif

class TemplateScript13_API TemplateScript13 : public Script
{
public:
	inline virtual TemplateScript13* Clone() const
	{
		return new TemplateScript13(*this);
	}
	void Start() override;
};

#endif __TemplateScript13_h__
