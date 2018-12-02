#include "ModuleFileSystem.h"
#include <assert.h>
#include <iostream>
#include <sys/stat.h>
#include <direct.h>
#include "SceneImporter.h"

#define ASSETS "Assets/"
#define LIBRARY "Library/"
#define MESHES "Meshes/"
#define MATERIALS "Materials/"

ModuleFileSystem::ModuleFileSystem()
{
}


ModuleFileSystem::~ModuleFileSystem()
{
}

unsigned int ModuleFileSystem::Load(const char * path, const char * file, char ** buffer) const
{
	const char *extension = GetExtension(file);
	if (extension == "png" || extension == "jpg")
	{
		//call matimporter
	}
	else if (extension == "dds")
	{
		//copy file if not in library/materials
	}
	else if (extension == "fbx" || extension == "FBX")
	{
		//save to library/meshes as .dat
	}
	else if (extension == "dat")
	{
		//move to library/meshes 
	}
	//SceneImporter sceneImporter;
	//sceneImporter.ImportMat("Models\\Baker\\Baker_house.png");
	return 0;
}

unsigned int ModuleFileSystem::Save(const char * file, const char * buffer, unsigned size) const
{
	return 0;
}

bool ModuleFileSystem::Remove(const char * file) const
{
	assert(file != nullptr);
	if (file == nullptr) return false;

	if (remove(file) != 0)
	{
		LOG("Error removing %s", file);
		return false;
	}
	LOG("%s successfully removed", file);
	return true;
}

bool ModuleFileSystem::Exists(const char * myfile) const
{
	if (FILE *file = fopen(myfile, "r")) {
		fclose(file);
		return true;
	}
	else {
		return false;
	}
}

bool ModuleFileSystem::MakeDirectory(const char * directory)
{
#ifdef __linux__
	if (mkdir(directory, 777) != 0)
	{
		LOG("Eror on creating directory %s", directory);
		return false
	}
	return true;
#else
	if (_mkdir(directory) != 0)
	{
		LOG("Eror on creating directory %s", directory);
		return false;
	}
	return true;
#endif
}

bool ModuleFileSystem::IsDirectory(const char * file) const
{
	struct stat info;
	if (stat(file, &info) != 0)
	{
		LOG("cannot access %s\n", file);
		return false;
	}
	else if (info.st_mode & S_IFDIR)
	{
		LOG("%s is a directory\n", file);
		return true;
	}
	else
	{
		LOG("%s is no directory\n", file);
		return false;
	}
}

const char * ModuleFileSystem::GetExtension(const char *file) const
{
	std::string filename(file);
	std::size_t found = filename.find_last_of(".");
	if (std::string::npos != found)
	{
		filename.erase(0, found + 1);
	}
	return filename.c_str();
}


