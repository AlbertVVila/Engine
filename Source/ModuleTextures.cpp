#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h" 
#include "ResourceTexture.h"

#include "GL/glew.h"
#include "IL/ilut.h"
#include "IL/ilu.h"
#include "imgui.h"
#include "JSON.h"

ModuleTextures::ModuleTextures()
{
}

// Destructor
ModuleTextures::~ModuleTextures()
{
	ilShutDown();
}

// Called before render is available
bool ModuleTextures::Init(JSON * config)
{
	LOG("Init Image library");
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	JSON_value* textureConfig = config->GetValue("textures");
	if (textureConfig == nullptr) return true;

	filter_type = (FILTERTYPE)textureConfig->GetUint("filter");

	return true;
}

// Called before quitting
bool ModuleTextures::CleanUp()
{
	LOG("Freeing textures and Image library");
	return true;
}

void ModuleTextures::SaveConfig(JSON * config)
{
	JSON_value* textureConfig = config->CreateValue();

	textureConfig->AddUint("filter", (unsigned)filter_type);
	config->AddValue("textures", *textureConfig);
}

void ModuleTextures::DrawGUI()
{
	ImGui::Text("Filter type on load:");
	ImGui::RadioButton("Linear", (int*)&filter_type, (unsigned)FILTERTYPE::LINEAR);
	ImGui::RadioButton("Nearest", (int*)&filter_type, (unsigned)FILTERTYPE::NEAREST);
	ImGui::RadioButton("Nearest MipMap", (int*)&filter_type, (unsigned)FILTERTYPE::NEAREST_MIPMAP_NEAREST);
	ImGui::RadioButton("Linear MipMap", (int*)&filter_type, (unsigned)FILTERTYPE::LINEAR_MIPMAP_LINEAR);
}

bool ModuleTextures::ImportImage(const char* file, const char* folder, ResourceTexture* resource) const
{
	ILuint imageID;
	ILboolean success;
	ILenum error;

	ilGenImages(1, &imageID); 		// Generate the image ID
	ilBindImage(imageID); 			// Bind the image
	std::string path(folder);
	success = ilLoadImage((path+file).c_str());
	if (success)
	{
		ILuint size;
		ILubyte* data = ilGetData();
		ilSetInteger(IL_DXTC_FORMAT, (ILint)resource->dxtFormat);	// To pick a specific DXT compression use
		size = ilSaveL(IL_DDS, NULL, 0);		// Get the size of the data buffer
		data = new ILubyte[size];				// allocate data buffer
		if (ilSaveL(IL_DDS, data, size) > 0)
		{
			// Save to buffer with the ilSaveIL function
			std::string filepath(TEXTURES);
			filepath += App->fsystem->RemoveExtension(file);
			filepath += TEXTUREEXT;
			App->fsystem->Save(filepath.c_str(), (char*)data, size);

			// Save image data to resource
			resource->width = ilGetInteger(IL_IMAGE_WIDTH);
			resource->height = ilGetInteger(IL_IMAGE_HEIGHT);
			resource->depth = ilGetInteger(IL_IMAGE_DEPTH);
			resource->format = ilGetInteger(IL_IMAGE_FORMAT);
			resource->bytes = ilGetInteger(GL_UNSIGNED_BYTE);
		}
		else
		{
			success = false;
		}
		ilDeleteImages(1, &imageID);
		RELEASE_ARRAY(data);
	}
	
	if(!success)
	{
		error = ilGetError();
		LOG("Error loading file %s, error: %s", file, iluErrorString(error));
	}
	return success;
}

void ModuleTextures::DrawImportConfiguration(ResourceTexture* resource)
{
	const char* compressionTypes[] = { "DXT1", /*"DXT2",*/ "DXT3", /*"DXT4",*/ "DXT5", /*"DXT_NO_COMP", "KEEP_DXTC_DATA", "DXTC_DATA_FORMAT",*/ "THREE_DC", "RXGB", "ATI1N", "DXT1A"};
	if (ImGui::Combo("Compression type", &resource->compression, compressionTypes, IM_ARRAYSIZE(compressionTypes)))
	{
		switch (resource->compression)
		{
		case 0:	resource->dxtFormat = DXT::DXT1; break;
		//case 1:	resource->dxtFormat = DXT::DXT2; break;
		case 1:	resource->dxtFormat = DXT::DXT3; break;
		//case 3:	resource->dxtFormat = DXT::DXT4; break;
		case 2:	resource->dxtFormat = DXT::DXT5; break;
		//case 5: resource->dxtFormat = DXT::DXT_NO_COMP; break;
		//case 3:	resource->dxtFormat = DXT::KEEP_DXTC_DATA; break;
		//case 4:	resource->dxtFormat = DXT::DXTC_DATA_FORMAT; break;
		case 3:	resource->dxtFormat = DXT::THREE_DC; break;
		case 4:	resource->dxtFormat = DXT::RXGB; break;
		case 5:	resource->dxtFormat = DXT::ATI1N; break;
		case 6:	resource->dxtFormat = DXT::DXT1A; break;
		}
	}
}