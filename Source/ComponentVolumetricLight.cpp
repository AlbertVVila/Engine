#include "ComponentVolumetricLight.h"

#include "Globals.h"
#include "Application.h"
#include "GameObject.h"

#include "imgui.h"

ComponentVolumetricLight::ComponentVolumetricLight(GameObject* gameobject) : Component(gameobject, ComponentType::VolumetricLight)
{
	Init();
}

ComponentVolumetricLight::ComponentVolumetricLight(const ComponentVolumetricLight& copy) : Component(copy)
{
}

Component* ComponentVolumetricLight::Clone() const
{
	return nullptr;
}

void ComponentVolumetricLight::Update()
{
}

void ComponentVolumetricLight::DrawProperties()
{
}

void ComponentVolumetricLight::Save(JSON_value* value) const
{
}

void ComponentVolumetricLight::Load(JSON_value* value)
{
}

void ComponentVolumetricLight::Init()
{
	float cPoints[192] = { //32 vertices circle of radius 1 * 2
		//Circle 1
		1.000f, 0.000f, 0.000f,
		0.981f, -0.000f, -0.195f,
		0.924f, -0.000f, -0.383f,
		0.831f, -0.000f, -0.556f,
		0.707f, -0.000f, -0.707f,
		0.556f, -0.000f, -0.831f,
		0.383f, -0.000f, -0.924f,
		0.195f, -0.000f, -0.981f,
		-0.000f, -0.000f, -1.000f,
		-0.195f, -0.000f, -0.981f,
		-0.383f, -0.000f, -0.924f,
		-0.556f, -0.000f, -0.831f,
		-0.707f, -0.000f, -0.707f,
		-0.831f, -0.000f, -0.556f,
		-0.924f, -0.000f, -0.383f,
		-0.981f, -0.000f, -0.195f,
		-1.000f, 0.000f, 0.000f,
		-0.981f, 0.000f, 0.195f,
		-0.924f, 0.000f, 0.383f,
		-0.831f, 0.000f, 0.556f,
		-0.707f, 0.000f, 0.707f,
		-0.556f, 0.000f, 0.831f,
		-0.383f, 0.000f, 0.924f,
		-0.195f, 0.000f, 0.981f,
		0.000f, 0.000f, 1.000f,
		0.195f, 0.000f, 0.981f,
		0.383f, 0.000f, 0.924f,
		0.556f, 0.000f, 0.831f,
		0.707f, 0.000f, 0.707f,
		0.831f, 0.000f, 0.556f,
		0.924f, 0.000f, 0.383f,
		// Circle2
		1.000f, 0.000f, 0.000f,
		0.981f, -0.000f, -0.195f,
		0.924f, -0.000f, -0.383f,
		0.831f, -0.000f, -0.556f,
		0.707f, -0.000f, -0.707f,
		0.556f, -0.000f, -0.831f,
		0.383f, -0.000f, -0.924f,
		0.195f, -0.000f, -0.981f,
		-0.000f, -0.000f, -1.000f,
		-0.195f, -0.000f, -0.981f,
		-0.383f, -0.000f, -0.924f,
		-0.556f, -0.000f, -0.831f,
		-0.707f, -0.000f, -0.707f,
		-0.831f, -0.000f, -0.556f,
		-0.924f, -0.000f, -0.383f,
		-0.981f, -0.000f, -0.195f,
		-1.000f, 0.000f, 0.000f,
		-0.981f, 0.000f, 0.195f,
		-0.924f, 0.000f, 0.383f,
		-0.831f, 0.000f, 0.556f,
		-0.707f, 0.000f, 0.707f,
		-0.556f, 0.000f, 0.831f,
		-0.383f, 0.000f, 0.924f,
		-0.195f, 0.000f, 0.981f,
		0.000f, 0.000f, 1.000f,
		0.195f, 0.000f, 0.981f,
		0.383f, 0.000f, 0.924f,
		0.556f, 0.000f, 0.831f,
		0.707f, 0.000f, 0.707f,
		0.831f, 0.000f, 0.556f,
		0.924f, 0.000f, 0.383f
	};
	memcpy_s(&conePoints[0], sizeof(float) * 192, &cPoints[0], sizeof(float) * 192);

	for (unsigned i = 0u; i < 64u; i += 2u)
	{
		coneUVs[i * 2u] = i / 64.f; //Circle1 s coord
		coneUVs[(i * 2u) + 1u] = 1.f; //Circle1 t coord
		coneUVs[(i * 2u) + 64u] = i / 64.f; //Circle2 s coord
		coneUVs[(i * 2u) + 65u] = 0.f; //Circle1 t coord
	}

	coneIndexes[0] = 0;
	coneIndexes[1] = 32;
	coneIndexes[2] = 33;
	coneIndexes[63] = 32;
	coneIndexes[64] = 0;

	int c1Indexer = 1;
	int c2Indexer = 34;

	bool c1 = true;
	for (unsigned i = 3u; i < 63u; ++i)
	{
		if (c1)
		{
			coneIndexes[i] = c1Indexer++;
		}
		else
		{
			coneIndexes[i] = c2Indexer++;
		}
		c1 = !c1;
	}
	int kk = 0;
}

