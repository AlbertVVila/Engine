#include "ModuleFileSystem.h"
#include <assert.h>
#include "SceneImporter.h"
#include "physfs.h"

#define ASSETS "Assets/"
#define LIBRARY "Library/"
#define MESHES "Meshes/"
#define MATERIALS "Materials/"
#define SKYBOX "Skybox/"
#define DATADIR "Data"

ModuleFileSystem::ModuleFileSystem()
{
}


ModuleFileSystem::~ModuleFileSystem()
{
	PHYSFS_deinit(); //TODO: Add to list of libraries
}

bool ModuleFileSystem::Init()
{
	PHYSFS_init(NULL);
	std::string dir(PHYSFS_getBaseDir());
	dir += DATADIR;
	PHYSFS_addToSearchPath(dir.c_str(), 1);
	PHYSFS_setWriteDir(dir.c_str());
	return true;
}

unsigned int ModuleFileSystem::Load(const char * path, const char * file, char ** buffer) const
{
	return 0;
}

bool ModuleFileSystem::Save(const char * file, const char * buffer, unsigned size) const
{
	PHYSFS_file* myfile = PHYSFS_openWrite(file);
	if (myfile == nullptr)
	{
		LOG("Error: %s", PHYSFS_getLastError());
		return false;
	}
	PHYSFS_write(myfile, buffer, size, 1);
	PHYSFS_close(myfile);

	return true;
}

bool ModuleFileSystem::Remove(const char * file) const
{
	assert(file != nullptr);
	if (file == nullptr) return false;
	if (PHYSFS_unmount(file) != 0)
	{
		LOG("Error: %s", PHYSFS_getLastError());
		return false;
	}
	return true;
}

bool ModuleFileSystem::Exists(const char * file) const
{
	assert(file != nullptr);
	if (file == nullptr) return false;
	return PHYSFS_exists(file);
}

unsigned ModuleFileSystem::Size(const char * file) const
{
	assert(file != nullptr);
	if (file == nullptr) return 0;
	PHYSFS_file* myfile = PHYSFS_openRead(file);
	PHYSFS_sint64 file_size = PHYSFS_fileLength(myfile);
	PHYSFS_close(myfile);
	return file_size;
}

bool ModuleFileSystem::MakeDirectory(const char * directory)
{
	assert(directory != nullptr);
	if (directory == nullptr) return false;
	int result = PHYSFS_mkdir(directory);
	if (result != 0)
	{
		LOG("Error: %s",PHYSFS_getLastError());
		return false;
	}

	return true;
}

bool ModuleFileSystem::IsDirectory(const char * file) const
{
	assert(file != nullptr);
	if (file == nullptr) return false;
	return PHYSFS_isDirectory(file);
}

std::list<std::string> ModuleFileSystem::ListFiles(const char * dir) const
{
	std::list<std::string> files;
	char **rc = PHYSFS_enumerateFiles(dir);
	char **i;
	
	for (i = rc; *i != NULL; i++)
		files.emplace_back(*rc);
	
	 PHYSFS_freeList(rc);
	 return files;
}

//const char * ModuleFileSystem::GetExtension(const char *file) const
//{
//	std::string filename(file);
//	std::size_t found = filename.find_last_of(".");
//	if (std::string::npos != found)
//	{
//		filename.erase(0, found + 1);
//	}
//	return filename.c_str();
//}


