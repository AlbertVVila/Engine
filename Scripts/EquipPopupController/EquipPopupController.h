#ifndef  __EquipPopupController_h__
#define  __EquipPopupController_h__

#include "BaseScript.h"

#ifdef EquipPopupController_EXPORTS
#define EquipPopupController_API __declspec(dllexport)
#else
#define EquipPopupController_API __declspec(dllimport)
#endif

class EquipPopupController_API EquipPopupController : public Script
{
};

#endif __EquipPopupController_h__
