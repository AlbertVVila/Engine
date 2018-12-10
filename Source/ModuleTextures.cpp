#include "Application.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "GL/glew.h"
#include "IL/ilut.h"
#include "imgui.h"


using namespace std;

ModuleTextures::ModuleTextures()
{
}

// Destructor
ModuleTextures::~ModuleTextures()
{

}

// Called before render is available
bool ModuleTextures::Init()
{
	LOG("Init Image library");
	ilutRenderer(ILUT_OPENGL);
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	return true;
}

// Called before quitting
bool ModuleTextures::CleanUp()
{
	LOG("Freeing textures and Image library");
	return true;
}

void ModuleTextures::DrawGUI()
{
	ImGui::Text("Filter type on load:");
	ImGui::RadioButton("Linear", &filter_type, LINEAR); ImGui::SameLine();
	ImGui::RadioButton("Nearest", &filter_type, NEAREST);
}

Texture * ModuleTextures::Load(const char * file) const //TODO: refactor texture load
{
	assert(file != NULL);
	ILuint imageID;
	ILboolean success;
	ILenum error;
	int width = 0;
	int height = 0;
	int pixelDepth = 0;
	int format = 0;

	char *data;
	std::string filename(file);
	unsigned size = App->fsystem->Load((MATERIALS + filename + MATERIALEXTENSION).c_str(), &data); //TODO: use mini resource maanger to optimize this
	ilGenImages(1, &imageID); 		// Generate the image ID
	ilBindImage(imageID); 			// Bind the image
	success = ilLoadL(IL_DDS, data, size);
	if (success)
	{
		GLuint textureID = 0;
		glGenTextures(1, &textureID);

		glBindTexture(GL_TEXTURE_2D, textureID);


		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		ILubyte* data = ilGetData();
		width = ilGetInteger(IL_IMAGE_WIDTH);
		height = ilGetInteger(IL_IMAGE_HEIGHT);
		pixelDepth = ilGetInteger(IL_IMAGE_DEPTH);
		format = ilGetInteger(IL_IMAGE_FORMAT);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		if (filter_type == LINEAR)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}

		ilDeleteImages(1, &imageID);

		glBindTexture(GL_TEXTURE_2D, 0);

		return new Texture(textureID, width, height);
	}
	else
	{
		error = ilGetError();
		LOG("Error loading data: %s\n", iluErrorString(error));
	}
	return nullptr;
}

unsigned int ModuleTextures::LoadCubeMap(const std::vector<std::string> &faces) //TODO: change to array[6]
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	int width = 0;
	int height = 0;
	int pixelDepth = 0;
	int format = 0;

	for (unsigned int i=0; i< faces.size(); ++i)
	{
		ILuint imageID;
		ILboolean success;
		ILenum error;
		ILinfo ImageInfo;

		ilGenImages(1, &imageID); 		// Generate the image ID
		ilBindImage(imageID); 			// Bind the image

		char *data;
		unsigned size = App->fsystem->Load((MATERIALS + faces[i] + MATERIALEXTENSION).c_str(), &data); //TODO: use mini resource maanger to optimize this
		success = ilLoadL(IL_DDS, data, size);
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

void ModuleTextures::ImportImage(const char * file, const char* folder)
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

		//ILinfo ImageInfo;
		//iluGetImageInfo(&ImageInfo);
		//if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		//{
		//	iluFlipImage();
		//}
		ILuint size;
		ILubyte* data = ilGetData();
		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
		size = ilSaveL(IL_DDS, NULL, 0);	// Get the size of the data buffer
		data = new ILubyte[size];// allocate data buffer
		if (ilSaveL(IL_DDS, data, size) > 0)
		{
			// Save to buffer with the ilSaveIL function
			std::string filepath(MATERIALS);
			filepath += App->fsystem->RemoveExtension(file);
			filepath += MATERIALEXTENSION;
			App->fsystem->Save(filepath.c_str(), (char*)data, size);
		}
		RELEASE_ARRAY(data);
	}
}


