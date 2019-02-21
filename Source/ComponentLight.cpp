#include "Application.h"

#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ModuleSpacePartitioning.h"

#include "ComponentLight.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"

#include "GameObject.h"
#include "Imgui/imgui.h"
#include "JSON.h"
#include "GL/glew.h"
#include "Geometry/Line.h"
#include "Geometry/LineSegment.h"
#include "Geometry/Circle.h"
#include "Math/MathFunc.h"
#include "MathGeoLib/include/Geometry/AABB.h"
#include "debugdraw.h"
#include "AABBTree.h"

#define INITIAL_RADIUS 1000.f

ComponentLight::ComponentLight(GameObject * gameobject) : Component(gameobject, ComponentType::Light)
{
	pointSphere.r = INITIAL_RADIUS;
}

ComponentLight::ComponentLight(const ComponentLight & component) : Component(component)
{
	position = component.position;
	direction = component.direction;
	color = component.color;

	inner = component.inner;
	outer = component.outer;
	App->scene->lights.push_back(this);
}

ComponentLight::~ComponentLight()
{
	App->scene->lights.remove(this);
}

void ComponentLight::Update() 
{
	if (gameobject->transform == nullptr) return;
	position = gameobject->transform->global.TranslatePart(); 

	direction = -(gameobject->transform->rotation*float3::unitZ).Normalized();
}

void ComponentLight::DrawProperties()
{
	if (ImGui::CollapsingHeader("Light"))
	{
		bool removed = Component::DrawComponentState();
		if (removed)
		{
			return;
		}
		ImGui::Separator();
		ImGui::Text("Type");
		const char * types[] = {"Directional","Point", "Spot"};
		if (ImGui::BeginCombo("",types[(int)lightType]))
		{
			for (int n = 0; n < LIGHTTYPES; n++)
			{
				bool is_selected = ((int)lightType == n);
				if (ImGui::Selectable(types[n], is_selected) && (int)lightType != n)
				{
					lightType = (LightType)n;
					ResetValues();
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		

		ImGui::ColorEdit3("Color", (float*)&color);

		if (lightType != LightType::DIRECTIONAL)
		{
			ImGui::Text("Attenuation");
			if (lightType == LightType::POINT)
				ImGui::DragFloat("Radius", &pointSphere.r);
			else
				ImGui::DragFloat("Range", &range);

			ImGui::DragFloat("Intensity", &intensity);
		}

		if (lightType == LightType::SPOT)
		{
			ImGui::Text("Angle");
			ImGui::DragFloat("Inner", (float*)&inner, 0.1f, 0.f, 90.f);
			ImGui::DragFloat("Outer", (float*)&outer, 0.1f, 0.f, 90.f);
		}
	}
}

void ComponentLight::DrawDebugLight() const
{
	DrawDebug();
	App->spacePartitioning->aabbTreeLighting.Draw();
}

void ComponentLight::Load(const JSON_value & value)
{
	Component::Load(value);
	if (gameobject->transform == nullptr) return;

	lightType = (LightType)value.GetUint("Lighttype");
	color = value.GetColor3("color");
	position = gameobject->transform->position;
	direction = gameobject->transform->rotation*float3::unitZ;

	if (lightType != LightType::DIRECTIONAL)
	{
		pointSphere.r = value.GetFloat("radius");
		intensity = value.GetFloat("intensity");		
		range = value.GetFloat("range");		
	}

	if (lightType == LightType::SPOT)
	{
		inner = value.GetFloat("inner");
		outer = value.GetFloat("outer");
	}
}

void ComponentLight::Save(JSON_value * value) const
{
	Component::Save(value);

	value->AddUint("Lighttype", (unsigned)lightType);
	value->AddFloat3("color", color);

	if (lightType != LightType::DIRECTIONAL)
	{
		value->AddFloat("radius", pointSphere.r);
		value->AddFloat("intensity", intensity);
		value->AddFloat("range", range);
	}

	if (lightType == LightType::SPOT)
	{
		value->AddFloat("inner", inner);
		value->AddFloat("outer", outer);
	}
}

ComponentLight * ComponentLight::Clone() const
{
	return new ComponentLight(*this);
}

void ComponentLight::ResetValues()
{
	float polar = 0.f; 
	float azimuth = 0.f; 
	color = float3::one;
	inner = 20.f;
	outer = 25.f;
}

void ComponentLight::DrawDebug() const
{
	switch (lightType)
	{
	case LightType::POINT:
		dd::sphere(pointSphere.pos, dd::colors::Gold, pointSphere.r);
		dd::aabb(gameobject->bbox.minPoint, gameobject->bbox.maxPoint, dd::colors::BurlyWood);
		break;
	case LightType::SPOT:
		dd::cone(gameobject->transform->GetGlobalPosition(), direction * range, dd::colors::Gold, spotEndRadius, .01f);
		dd::aabb(gameobject->bbox.minPoint, gameobject->bbox.maxPoint, dd::colors::BurlyWood);	
	}
}

void ComponentLight::CalculateGuizmos()
{
	if (gameobject != nullptr)
	{
		switch (lightType)
		{
		case LightType::POINT:
			pointSphere.pos = gameobject->transform->GetGlobalPosition();
			gameobject->bbox.SetNegativeInfinity();
			gameobject->bbox.Enclose(pointSphere);
			break;
		case LightType::SPOT:	
		{
			spotEndRadius = range * tan(DegToRad(outer));
			pointSphere.pos = gameobject->transform->GetGlobalPosition();
			pointSphere.r = sqrt(pow(spotEndRadius, 2) + pow(range, 2));
			gameobject->bbox.SetNegativeInfinity();
			float3 p0 = pointSphere.pos + gameobject->transform->front * range;
			float3 p1 = p0 + gameobject->transform->right * spotEndRadius;
			float3 p2 = p0 - gameobject->transform->right * spotEndRadius;
			float3 p3 = p0 + gameobject->transform->up * spotEndRadius;
			float3 p4 = p0 - gameobject->transform->up * spotEndRadius;
			float3 points[6] = { pointSphere.pos, p0, p1, p2, p3, p4 };
			gameobject->bbox.SetFrom(points, 6);
			break;
		}
		case LightType::DIRECTIONAL:			
			if (App->scene->maincamera != nullptr)
			{
				pointSphere.pos = App->scene->maincamera->frustum->pos;
				pointSphere.r = App->scene->maincamera->frustum->farPlaneDistance;
			}
			else
			{
				pointSphere.pos = App->camera->editorcamera->frustum->pos;
				pointSphere.r = App->camera->editorcamera->frustum->farPlaneDistance;
			}
			gameobject->bbox.SetNegativeInfinity();
			gameobject->bbox.Enclose(pointSphere);
			break;
		}		
	}
}


