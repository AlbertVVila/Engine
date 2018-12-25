#include "Application.h"

#include "ModuleResourceManager.h"

#include "ComponentRenderer.h"

#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"

ComponentRenderer::ComponentRenderer(GameObject * gameobject) : Component(gameobject, ComponentType::Renderer)
{
	mesh = new Mesh();
	material = new Material(); //set default mat
}

ComponentRenderer::ComponentRenderer(const ComponentRenderer & component) : Component(component)
{
}

ComponentRenderer::~ComponentRenderer()
{
}

Component * ComponentRenderer::Clone() const
{
	return nullptr;
}

void ComponentRenderer::DrawProperties()
{

}

void ComponentRenderer::Save(JSON_value * value) const
{
}

void ComponentRenderer::Load(const JSON_value & value)
{
}

void ComponentRenderer::SetMesh(unsigned uid)
{
	if (uid != 0)
	{

	}
}

void ComponentRenderer::SetMaterial(const char * materialfile)
{
	assert(materialfile != nullptr); assert(material != nullptr);

	App->resManager->DeleteMaterial(material->name);
	material = App->resManager->GetMaterial(materialfile);
	if (material == nullptr)
	{
		material = new Material();
		material->Load(materialfile);
		App->resManager->AddMaterial(material);
		return;
	}
	return;
}

