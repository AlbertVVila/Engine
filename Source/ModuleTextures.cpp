#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h" 
#include "ResourceTexture.h"
#include "rapidjson/document.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"

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
		LOG("Imported image %s", file);
		ILuint size;
		ILubyte* data = ilGetData();
		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);	// To pick a specific DXT compression use
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

void ModuleTextures::SaveMetafile(const char* file, ResourceTexture* resource)
{
	std::string filepath;
	filepath.append(file);
	JSON *json = new JSON();
	rapidjson::Document* meta = new rapidjson::Document();
	rapidjson::Document::AllocatorType& alloc = meta->GetAllocator();
	filepath += ".meta";
	App->fsystem->Save(filepath.c_str(), json->ToString().c_str(), json->Size());
	struct stat statFile;
	stat(filepath.c_str(), &statFile);
	FILE* fp = fopen(filepath.c_str(), "wb");
	char writeBuffer[65536];
	rapidjson::FileWriteStream* os = new rapidjson::FileWriteStream(fp, writeBuffer, sizeof(writeBuffer));
	rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(*os);
	meta->SetObject();
	meta->AddMember("GUID", resource->GetUID(), alloc);
	meta->AddMember("timeCreated",statFile.st_ctime, alloc);
	meta->AddMember("height", resource->height, alloc);
	meta->AddMember("width", resource->width, alloc);
	meta->AddMember("depth", resource->depth, alloc);
	meta->AddMember("mips", resource->mips, alloc);
	meta->AddMember("format", resource->format, alloc);
	meta->AddMember("DX compresion", ilGetInteger(IL_DXTC_FORMAT), alloc);
	meta->AddMember("mipmap", ilGetInteger(IL_ACTIVE_MIPMAP), alloc);
	meta->Accept(writer);
	fclose(fp);
}
