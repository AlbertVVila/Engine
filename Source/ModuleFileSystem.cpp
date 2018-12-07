#include "ModuleFileSystem.h"
#include <assert.h>
#include "FileImporter.h"
#include "physfs.h"

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

bool ModuleFileSystem::Start()
{
	importTimer.Start();
	return true;
}

update_status ModuleFileSystem::Update()
{
	if (importTimer.Read() > watchThreshold)
	{
		WatchFolder(ASSETS);
		importTimer.Start();
	}
	return UPDATE_CONTINUE;
}

unsigned ModuleFileSystem::Load(const char * file, char ** buffer) const
{
	PHYSFS_file* myfile = PHYSFS_openRead(file);
	if (myfile == nullptr)
	{
		LOG("Error: %s", PHYSFS_getLastError());
		return false;
	}
	PHYSFS_sint32 fileSize = PHYSFS_fileLength(myfile);

	*buffer = new char[fileSize+1]();
	int readed =PHYSFS_read(myfile, *buffer, 1, fileSize);
	if (readed != fileSize)
	{
		LOG("Error reading from file %s, : %s",file, PHYSFS_getLastError());
		RELEASE(*buffer);
		return 0;
	}
	PHYSFS_close(myfile);

	return readed;
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

bool ModuleFileSystem::CopyFromOutsideFS(const char * source, const char * destination)
{
	char *data;
	FILE* fp = fopen(source, "rb");
	if (!fp) {
		LOG("File opening failed");
		return false;
	}
	fseek(fp, 0, SEEK_END);
	unsigned size = ftell(fp);
	data = new char[size];
	if (size != fread_s(data, size,1 , size, fp))
	{
		LOG("Error reading from file %s, : %s", source, PHYSFS_getLastError());
		RELEASE(data);
		return false;
	}
	Save(destination, data, size);
	return true;
}

void ModuleFileSystem::WatchFolder(const char * folder)
{
	std::list<std::string> files = ListFiles(folder);
	for (auto& file: files)
	{
		std::set<const char*>::iterator it = importedFiles.find(file.c_str());
		if (it == importedFiles.end())
		{
			FileImporter importer;
			importer.Import("Assets/test.dds");
			importedFiles.insert(file.c_str());
		}
	}
	return;
}

std::string ModuleFileSystem::GetExtension(const char *file) const
{
	std::string filename(file);
	std::size_t found = filename.find_last_of(".");
	if (std::string::npos != found)
	{
		filename.erase(0, found + 1);
	}
	return filename;
}


