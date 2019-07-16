#include "ComponentVolumetricLight.h"

#include "Globals.h"
#include "Application.h"
#include "GameObject.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"

#include "ComponentRenderer.h"
#include "imgui.h"

ComponentVolumetricLight::ComponentVolumetricLight(GameObject* gameobject) : Component(gameobject, ComponentType::VolumetricLight)
{
	Init();
	renderer = new ComponentRenderer(gameobject);
	renderer->isVolumetricLight = true;	
}

ComponentVolumetricLight::ComponentVolumetricLight(const ComponentVolumetricLight& copy) : Component(copy)
{
}

ComponentVolumetricLight::~ComponentVolumetricLight()
{
	RELEASE(mesh);
	RELEASE(renderer);
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
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Volumetric Light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::DragFloat("Circle 1 radius", &circle1Radius, 1.f, 0.f, MAX_RADIUS) || ImGui::DragFloat("Circle 2 radius", &circle2Radius, 1.f, 0.f, MAX_RADIUS))
			UpdateMesh();

		renderer->DrawProperties();		
	}
	ImGui::PopID();
}

void ComponentVolumetricLight::Save(JSON_value* value) const
{
}

void ComponentVolumetricLight::Load(JSON_value* value)
{
}

void ComponentVolumetricLight::UpdateMesh()
{
	int halfCone = VERT_AMOUNT / 2;
	float cPoints[VERT_AMOUNT];
	gameobject->bbox.SetNegativeInfinity();
	for (int i = 0; i < halfCone; ++i)
	{
		cPoints[i] = conePoints[i] * circle1Radius;
		cPoints[i + halfCone] = conePoints[i + halfCone] * circle2Radius;
		if (i % 3 == 0)
		{
			gameobject->bbox.Enclose(math::float3(&cPoints[i]));
			gameobject->bbox.Enclose(math::float3(&cPoints[i + halfCone]));
		}
	}	
	if (mesh)
	{
		RELEASE(mesh);
		mesh = new ResourceMesh(VERT_AMOUNT, &conePoints[0], INDEX_AMOUNT, &coneIndexes[0], UV_AMOUNT, &coneUVs[0]);
	}
	renderer->mesh = mesh;
}

void ComponentVolumetricLight::Init()
{
	float cPoints[VERT_AMOUNT] = { //2 32 vertices circles of radius 1
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
	memcpy_s(&conePoints[0], sizeof(float) * VERT_AMOUNT, &cPoints[0], sizeof(float) * VERT_AMOUNT);

	for (unsigned i = 0u; i < INDEX_AMOUNT - 1; i += 2u)
	{
		coneUVs[i * 2u] = i / (float)(INDEX_AMOUNT - 1); //Circle1 s coord
		coneUVs[(i * 2u) + 1u] = 1.f; //Circle1 t coord
		coneUVs[(i * 2u) + 64u] = (float)(INDEX_AMOUNT - 1); //Circle2 s coord
		coneUVs[(i * 2u) + 65u] = 0.f; //Circle1 t coord
	}

	int halfIndex = INDEX_AMOUNT / 2;
	coneIndexes[0] = 0;
	coneIndexes[1] = halfIndex;
	coneIndexes[2] = halfIndex + 1;
	coneIndexes[INDEX_AMOUNT - 2] = halfIndex;
	coneIndexes[INDEX_AMOUNT - 1] = 0;

	int c1Indexer = 1;
	int c2Indexer = halfIndex + 2;

	bool c1 = true;
	for (unsigned i = 3u; i < INDEX_AMOUNT - 2; ++i)
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

