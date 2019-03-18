#include "ResourceMaterial.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleTextures.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"

#include "ResourceTexture.h"

#include "JSON.h"
#include "rapidjson/document.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "GL/glew.h"

ResourceMaterial::ResourceMaterial(unsigned uid) : Resource(uid, TYPE::MATERIAL)
{
}

ResourceMaterial::ResourceMaterial(const ResourceMaterial& resource) : Resource(resource)
{
	name = resource.name;
	if (resource.shader != nullptr)
	{
		shader = App->program->GetProgram(resource.shader->file.c_str());
	}
	for (unsigned i = 0; i < MAXTEXTURES; ++i)
	{
		if (resource.textures[i] != nullptr)
		{
			textures[i] = (ResourceTexture*)App->resManager->Get(resource.textures[i]->GetExportedFile());
		}
	}

	diffuseColor = resource.diffuseColor;
	specularColor = resource.specularColor;
	emissiveColor = resource.emissiveColor;

	kAmbient = resource.kAmbient;
	kDiffuse = resource.kDiffuse;
	kSpecular = resource.kSpecular;
	shininess = resource.shininess;
}

ResourceMaterial::~ResourceMaterial()
{
	DeleteFromMemory();
}

void ResourceMaterial::DeleteFromMemory()
{
	Resource::DeleteFromMemory();
	if (shader != nullptr)
	{
		App->resManager->DeleteProgram(shader->file);
		shader = nullptr;
	}
	for (unsigned i = 0; i < MAXTEXTURES; i++)
	{
		if (textures[i] != nullptr)
		{
			App->resManager->DeleteResource(textures[i]->GetUID());
			textures[i] = nullptr;
		}
	}
}

bool ResourceMaterial::LoadInMemory()
{
	char* data = nullptr;
	// Load JSON
	if (App->fsystem->Load((IMPORTED_MATERIALS + exportedFileName + MATERIALEXT).c_str(), &data) == 0)
		return false;

	JSON *json = new JSON(data);
	JSON_value *materialJSON = json->GetValue("material");

	name = exportedFileName;
	diffuseColor = materialJSON->GetColor4("diffuseColor");
	specularColor = materialJSON->GetColor3("specularColor");
	emissiveColor = materialJSON->GetColor3("emissiveColor");

	metallic = materialJSON->GetFloat("metallic");
	roughness = materialJSON->GetFloat("roughness");

	const char *diffuseFile = materialJSON->GetString("diffuse");
	if (diffuseFile != nullptr)
	{
		textures[(unsigned)TextureType::DIFFUSE] = (ResourceTexture*)App->resManager->Get(diffuseFile);
	}
	const char *specularFile = materialJSON->GetString("specular");
	if (specularFile != nullptr)
	{
		textures[(unsigned)TextureType::SPECULAR] = (ResourceTexture*)App->resManager->Get(specularFile);
	}
	const char *occlusionFile = materialJSON->GetString("occlusion");
	if (occlusionFile != nullptr)
	{
		textures[(unsigned)TextureType::OCCLUSION] = (ResourceTexture*)App->resManager->Get(occlusionFile);
	}
	const char *emissiveFile = materialJSON->GetString("emissive");
	if (emissiveFile != nullptr)
	{
		textures[(unsigned)TextureType::EMISSIVE] = (ResourceTexture*)App->resManager->Get(emissiveFile);
	}
	const char* normalFile = materialJSON->GetString("normal");
	if (normalFile != nullptr)
	{
		textures[(unsigned)TextureType::NORMAL] = (ResourceTexture*)App->resManager->Get(normalFile);
	}

	const char* shaderName = materialJSON->GetString("shader");
	if (shaderName != nullptr)
	{
		shader = App->program->GetProgram(materialJSON->GetString("shader"));
	}

	RELEASE_ARRAY(data);
	RELEASE(json);
	++loaded;
	return true;
}

void ResourceMaterial::Save() const
{
	JSON *json = new JSON();
	JSON_value *materialJSON = json->CreateValue();

	if (textures[(unsigned)TextureType::DIFFUSE] != nullptr)
		materialJSON->AddFloat4("diffuseColor", diffuseColor);

	if (textures[(unsigned)TextureType::SPECULAR] != nullptr)
		materialJSON->AddFloat3("specularColor", specularColor);

	if (textures[(unsigned)TextureType::EMISSIVE] != nullptr)
		materialJSON->AddFloat3("emissiveColor", emissiveColor);

	materialJSON->AddFloat("metallic", metallic);
	materialJSON->AddFloat("roughness", roughness);


	if (textures[(unsigned)TextureType::DIFFUSE] != nullptr)
	{
		materialJSON->AddString("diffuse", textures[(unsigned)TextureType::DIFFUSE]->GetExportedFile());
	}
	if (textures[(unsigned)TextureType::SPECULAR] != nullptr)
	{
		materialJSON->AddString("specular", textures[(unsigned)TextureType::SPECULAR]->GetExportedFile());
	}
	if (textures[(unsigned)TextureType::OCCLUSION] != nullptr)
	{
		materialJSON->AddString("occlusion", textures[(unsigned)TextureType::OCCLUSION]->GetExportedFile());
	}
	if (textures[(unsigned)TextureType::EMISSIVE] != nullptr)
	{
		materialJSON->AddString("emissive", textures[(unsigned)TextureType::EMISSIVE]->GetExportedFile());
	}
	if (textures[(unsigned)TextureType::NORMAL] != nullptr)
	{
		materialJSON->AddString("normal", textures[(unsigned)TextureType::NORMAL]->GetExportedFile());
	}

	if (shader != nullptr)
	{
		materialJSON->AddString("shader", shader->file.c_str());
	}
	
	json->AddValue("material", *materialJSON);

	App->fsystem->Save((MATERIALS + name + MATERIALEXT).c_str(), json->ToString().c_str(), json->Size());
	RELEASE(json);
}

void ResourceMaterial::SaveMetafile(const char* file) const
{
	std::string filepath;
	filepath.append(file);
	JSON *json = new JSON();
	rapidjson::Document* meta = new rapidjson::Document();
	rapidjson::Document::AllocatorType& alloc = meta->GetAllocator();
	filepath += ".meta";
	App->fsystem->Save(filepath.c_str(), json->ToString().c_str(), json->Size());
	FILE* fp = fopen(filepath.c_str(), "wb");
	char writeBuffer[65536];
	rapidjson::FileWriteStream* os = new rapidjson::FileWriteStream(fp, writeBuffer, sizeof(writeBuffer));
	rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(*os);
	meta->SetObject();
	meta->AddMember("GUID", GetUID(), alloc);
	meta->Accept(writer);
	fclose(fp);
}

void ResourceMaterial::Reset(const ResourceMaterial & material)
{
	name = material.name;

	if (shader != nullptr)
	{
		App->resManager->DeleteProgram(shader->file);
	}
	if (material.shader != nullptr)
	{
		shader = App->program->GetProgram(material.shader->file.c_str());
	}
	for (unsigned i = 0; i < MAXTEXTURES; ++i)
	{
		if (textures[i] != nullptr)
		{
			App->resManager->DeleteResource(textures[i]->GetUID());
		}
		if (material.textures[i] != nullptr)
		{
			textures[i] = (ResourceTexture*)App->resManager->Get(material.textures[i]->GetExportedFile());
		}
		else
		{
			textures[i] = nullptr;
		}
	}
	diffuseColor = material.diffuseColor;
	specularColor = material.specularColor;
	emissiveColor = material.emissiveColor;

	kAmbient = material.kAmbient;
	kDiffuse = material.kDiffuse;
	kSpecular = material.kSpecular;
	shininess = material.shininess;
}

int ResourceMaterial::Compare(const ResourceMaterial& material)
{
	if (name.compare(material.name) != 0)
		return -1;

	if (shader != material.shader)
		return 0;

	for (unsigned i = 0; i < MAXTEXTURES; ++i)
	{
		if (textures[i] != material.textures[i])
			return 0;
	}

	if (!diffuseColor.Equals(diffuseColor))
		return 0;
	if (!specularColor.Equals(specularColor))
		return 0;
	if (!emissiveColor.Equals(emissiveColor))
		return 0;

	if (kDiffuse != kDiffuse)
		return 0;
	if (kSpecular != kSpecular)
		return 0;
	if (kAmbient != kAmbient)
		return 0;

	if (shininess != shininess)
		return 0;

	return 1;
}

ResourceTexture* ResourceMaterial::GetTexture(TextureType type) const
{
	return textures[(unsigned)type];
}

std::list<ResourceTexture*> ResourceMaterial::GetTextures() const
{
	std::list<ResourceTexture*> mytextures;
	for (unsigned i = 0; i < MAXTEXTURES; i++)
	{
		if (textures[i] != nullptr)
		{
			mytextures.push_back(textures[i]);
		}
	}
	return mytextures;
}

void ResourceMaterial::SetUniforms(unsigned shader) const
{
	for (unsigned int i = 0; i < MAXTEXTURES; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		char* textureType = nullptr;
		float* color = (float*)&float3::zero;
		switch ((TextureType)i)
		{
		case TextureType::DIFFUSE:
			textureType = "diffuse";
			color = (float*)&diffuseColor;
			break;

		case TextureType::SPECULAR:
			textureType = "specular";
			color = (float*)&specularColor;
			break;

		case TextureType::OCCLUSION:
			textureType = "occlusion";
			break;

		case TextureType::EMISSIVE:
			textureType = "emissive";
			color = (float*)&emissiveColor;
			break;
		}

		char texture[32];
		sprintf(texture, "material.%s_texture", textureType);

		char uniform[32];
		sprintf(uniform, "material.%s_color", textureType);

		if (textures[i] != nullptr)
		{
			if (i == (unsigned)TextureType::DIFFUSE)
			{
				glUniform4fv(glGetUniformLocation(shader,
					uniform), 1, color);
			}
			else
			{
				glUniform3fv(glGetUniformLocation(shader,
					uniform), 1, color);
			}
			glBindTexture(GL_TEXTURE_2D, textures[i]->gpuID);

			glUniform1i(glGetUniformLocation(shader, texture), i);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, 0);
			glUniform1i(glGetUniformLocation(shader, texture), i);
			float3 noColor = float3::zero; //Used as a fallback

			glUniform3fv(glGetUniformLocation(shader,
				uniform), 1, (GLfloat*)&noColor);
		}
		glDisable(GL_TEXTURE_2D);
	}

	glUniform1fv(glGetUniformLocation(shader,
		"material.k_ambient"), 1, (GLfloat*)&kAmbient);
	glUniform1fv(glGetUniformLocation(shader,
		"material.k_diffuse"), 1, (GLfloat*)&kDiffuse);
	glUniform1fv(glGetUniformLocation(shader,
		"material.k_specular"), 1, (GLfloat*)&kSpecular);
	glUniform1fv(glGetUniformLocation(shader,
		"material.shininess"), 1, (GLfloat*)&shininess);
}