#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h" 
#include "ResourceTexture.h"

#include "GL/glew.h"
#include "IL/ilut.h"
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

ResourceTexture * ModuleTextures::GetTexture(const char * file) const
{
	assert(file != NULL);

	// Look for it on the resource list
	unsigned uid = App->resManager->Find(file);
	if (uid == 0) 
		return nullptr;
		
	// Check if is already loaded in memory
	ResourceTexture* textureResource = (ResourceTexture*)App->resManager->Get(uid);
	if (!textureResource->IsLoadedToMemory())
	{
		// Load in memory
		if (textureResource->LoadInMemory())
			return textureResource;
		else
			return nullptr;
	}
	else
	{
		textureResource->SetReferences(textureResource->GetReferences() + 1);
		return textureResource;
	}
}

unsigned ModuleTextures::LoadCubeMap(const std::string faces[]) const
{
	unsigned textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	unsigned width = 0;
	unsigned height = 0;
	unsigned pixelDepth = 0;
	int format = 0;

	for (unsigned int i=0; i< NUMFACES; ++i)
	{
		ILuint imageID;
		ILboolean success;
		ILenum error;

		ilGenImages(1, &imageID); 		// Generate the image ID
		ilBindImage(imageID); 			// Bind the image

		char *data;
		unsigned size = App->fsystem->Load((TEXTURES + faces[i] + TEXTUREEXT).c_str(), &data);
		success = ilLoadL(IL_DDS, data, size);
		RELEASE_ARRAY(data);

		if (success)
		{

			ILubyte* ildata = ilGetData();
			width = ilGetInteger(IL_IMAGE_WIDTH);
			height = ilGetInteger(IL_IMAGE_HEIGHT);
			pixelDepth = ilGetInteger(IL_IMAGE_DEPTH);
			format = ilGetInteger(IL_IMAGE_FORMAT);

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, format, width, height, 0, format, GL_UNSIGNED_BYTE, ildata);

			ilDeleteImages(1, &imageID);
		}
		else
		{
			error = ilGetError();
			LOG("Error file %s: %s\n", faces[i], iluErrorString(error));
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

bool ModuleTextures::ImportImage(const char* file, const char* folder, std::string& exportedFile, ResourceTexture* resource)
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
		LOG("Imported image %s", file);
		ILuint size;
		ILubyte* data = ilGetData();
		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
		size = ilSaveL(IL_DDS, NULL, 0);	// Get the size of the data buffer
		data = new ILubyte[size];// allocate data buffer
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
			exportedFile = App->fsystem->RemoveExtension(file);
			SaveMetafile(filepath, resource);
		}
		ilDeleteImages(1, &imageID);
		RELEASE_ARRAY(data);
	}
	else
	{
		error = ilGetError();
		LOG("Error loading file %s, error: %s\n", file, iluErrorString(error));
	}
	return success;
}

void ModuleTextures::SaveMetafile(std::string filepath, ResourceTexture* resource)
{
	/*std::string c = "guid: " + std::to_string(resource->GetUID()) + "\n";
	c += "width: " + std::to_string(resource->width) + "\n";
	c += "heigth: " + std::to_string(resource->height) + "\n";
	c += "depth: " + std::to_string(resource->depth) + "\n";
	c += "mips: " + std::to_string(resource->mips) + "\n";
	c += "format: " + std::to_string(resource->format) + "\n";
	filepath = App->fsystem->RemoveExtension(filepath);
	filepath += ".meta";
	App->fsystem->Save(filepath.c_str(), c.c_str(), c.size());*/

	JSON *json = new JSON();
	JSON_value *array = json->CreateValue();
	array->AddString("GUID",(std::to_string(resource->GetUID())).c_str());
	array->AddInt("width", resource->width);
	array->AddInt("height", resource->height);
	array->AddInt("depth", resource->depth);
	array->AddInt("mips", resource->mips);
	array->AddInt("format", resource->format);
	json->AddValue("Texture", *array);
	filepath += ".meta";
	App->fsystem->Save(filepath.c_str(), json->ToString().c_str(), json->Size());
}
