#include "ResourceTexture.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleTextures.h"

#include "GL/glew.h"
#include "IL/ilut.h"
#include "JSON.h"

ResourceTexture::ResourceTexture(unsigned uid) : Resource(uid, TYPE::TEXTURE)
{
}


ResourceTexture::~ResourceTexture()
{
	DeleteFromMemory();
}

bool ResourceTexture::LoadInMemory()
{
	if (Resource::IsLoadedToMemory())
		return false;

	ILboolean success;
	int format = 0;
	unsigned imageID;

	char *data;
	std::string filename(file);
	unsigned size = App->fsystem->Load((TEXTURES + filename).c_str(), &data);

	ilGenImages(1, &imageID); 		// Generate the image ID
	ilBindImage(imageID); 			// Bind the image
	success = ilLoadL(IL_DDS, data, size);
	RELEASE_ARRAY(data);

	if (success)
	{
		glGenTextures(1, &gpuID);

		glBindTexture(GL_TEXTURE_2D, gpuID);

		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		ILubyte* data = ilGetData();
		width = ilGetInteger(IL_IMAGE_WIDTH);
		height = ilGetInteger(IL_IMAGE_HEIGHT);
		depth = ilGetInteger(IL_IMAGE_DEPTH);
		format = ilGetInteger(IL_IMAGE_FORMAT);
		
		//TODO: Switch to save format variable

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		if (App->textures->filter_type == FILTERTYPE::LINEAR)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		else if (App->textures->filter_type == FILTERTYPE::NEAREST)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
		else if (App->textures->filter_type == FILTERTYPE::NEAREST_MIPMAP_NEAREST)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		ilDeleteImages(1, &gpuID);

		glBindTexture(GL_TEXTURE_2D, 0);
		return LoadToMemory();
	}
	else
	{
		ILenum error = ilGetError();
		LOG("Error loading data: %s\n", iluErrorString(error));
		return false;
	}
}

void ResourceTexture::DeleteFromMemory()
{
	Resource::DeleteFromMemory();
	if (gpuID != 0)
		glDeleteTextures(1, &gpuID);
}

void ResourceTexture::Save(JSON_value &config) const
{
	Resource::Save(config);

	config.AddUint("Width", width);
	config.AddUint("Height", height);
	config.AddUint("Depth", depth);
	config.AddUint("Mips", mips);
	config.AddUint("Bytes", bytes);
	config.AddUint("GpuID", gpuID);

	config.AddUint("Format", (unsigned)format);
}

void ResourceTexture::Load(const JSON_value &config)
{
	Resource::Load(config);

	width = config.GetUint("Width");
	height = config.GetUint("Height");
	depth = config.GetUint("Depth");
	mips = config.GetUint("Mips");
	bytes = config.GetUint("Bytes");
	gpuID = config.GetUint("GpuID");

	format = (FORMAT)config.GetUint("Format");
}