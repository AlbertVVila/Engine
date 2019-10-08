

#include "ThirdPhaseAOE.h"

ThirdPhaseAOE_API Script* CreateScript()
{
	ThirdPhaseAOE* instance = new ThirdPhaseAOE;
	return instance;
}

void ThirdPhaseAOE::Awake()
{

}

void ThirdPhaseAOE::Start()
{
	switch (aoeType)
	{
	case 0:
		break;
	case 1:
		break;
	}
}

void ThirdPhaseAOE::Update()
{
	switch (aoeType)
	{
	case 0:
		HandleFirstAOE();
		break;
	case 1:
		HandleReplicas();
		break;
	}
}

void ThirdPhaseAOE::Expose(ImGuiContext * context)
{
}

void ThirdPhaseAOE::Serialize(JSON_value * json) const
{
}

void ThirdPhaseAOE::DeSerialize(JSON_value * json)
{
}

void ThirdPhaseAOE::HandleFirstAOE()
{
}

void ThirdPhaseAOE::HandleReplicas()
{
}
