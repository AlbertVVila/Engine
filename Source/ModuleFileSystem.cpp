#include "ModuleFileSystem.h"
#include <assert.h>
#include "FileImporter.h"
#include "physfs.h"

ModuleFileSystem::ModuleFileSystem()
{
	PHYSFS_init(NULL);
	PHYSFS_addToSearchPath(PHYSFS_getBaseDir(), 1);

	PHYSFS_mount(LIBRARY, nullptr, 1);
	PHYSFS_mount(ASSETS, nullptr, 1);
	PHYSFS_mount(SHADERS, nullptr, 1);
	PHYSFS_setWriteDir(PHYSFS_getBaseDir());
}


ModuleFileSystem::~ModuleFileSystem()
{
	PHYSFS_deinit();
}

bool ModuleFileSystem::Start()
{
	CheckImportedFiles(LIBRARY);
	WatchFolder(ASSETS);
	importTimer.Start();
	return true;
}

update_status ModuleFileSystem::Update(float dt) //TODO: Separate thread
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
		LOG("Error: %s %s", file, PHYSFS_getLastError());
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
		LOG("Error: %s %s", file, PHYSFS_getLastError());
		return false;
	}
	PHYSFS_write(myfile, buffer, size, 1); //TODO: check errors in write
	//RELEASE(buffer); //TODO: fix mem leacks
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

bool ModuleFileSystem::MakeDirectory(const char * directory) const
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

std::vector<std::string> ModuleFileSystem::ListFiles(const char * dir, bool extension) const
{
	std::vector<std::string> files;
	char **rc = PHYSFS_enumerateFiles(dir);
	char **i;

	for (i = rc; *i != NULL; i++)
	{
		if (!extension)
		{
			files.emplace_back(RemoveExtension(*i));
		}
		else
		{
			files.emplace_back(*i);
		}
		//char *file = new char[strlen(*i)];
		//strcpy(file, *i);
		//files.emplace_back(file); //TODO: copy char*
	}

	PHYSFS_freeList(rc);
	return files;
}

bool ModuleFileSystem::CopyFromOutsideFS(const char * source, const char * destination) const
{
	char *data;
	FILE* fp = fopen(source, "rb");
	if (!fp) {
		LOG("File opening failed");
		return false;
	}
	fseek(fp, 0, SEEK_END);
	unsigned size = ftell(fp);
	rewind(fp);

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

bool ModuleFileSystem::Copy(const char * source, const char * destination, const char* file) const
{
	char * data;
	std::string filepath(source);
	filepath += file;
	unsigned size = Load(filepath.c_str(), &data);
	std::string filedest(destination);
	filedest += file;
	Save(filedest.c_str(), data, size);
	return true;
}

void ModuleFileSystem::CheckImportedFiles(const char * folder)//TODO: improve using extensions otherwise conflict between files with same name and diff extension
{
	std::vector<std::string> files = ListFiles(folder);
	for (auto& file : files)
	{
		std::string filefolder(folder);
		filefolder += file;
		if (IsDirectory(filefolder.c_str()))
		{
			CheckImportedFiles((filefolder + "/").c_str());
		}
		else
		{
			importedFiles.insert(RemoveExtension(file.c_str()));
		}
	}
}
void ModuleFileSystem::WatchFolder(const char * folder)
{
	std::vector<std::string> files = ListFiles(folder);
	for (auto& file: files)
	{
		std::string filefolder(folder);
		filefolder+=file;
		if (IsDirectory(filefolder.c_str()))
		{
			WatchFolder((filefolder + "/").c_str());
		}
		else
		{
			std::set<std::string>::iterator it = importedFiles.find(RemoveExtension(file.c_str()));
			if (it == importedFiles.end())
			{
				FileImporter importer;
				importer.ImportAsset(file.c_str(), folder);
				importedFiles.insert(RemoveExtension(file.c_str()));
			}
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
		filename.erase(0, found);
	}
	return filename;
}

std::string ModuleFileSystem::RemoveExtension(const char *file) const
{
	std::string filename(file);
	std::size_t found = filename.find_last_of(".");
	if (std::string::npos != found)
	{
		filename.erase(found, filename.size());
	}
	return filename;
}

std::string ModuleFileSystem::GetFilename(const char *file) const
{
	std::string filename(file);
	std::size_t found = filename.find_last_of(".");
	if (std::string::npos != found)
	{
		filename.erase(found, filename.size());
	}

	found = filename.find_last_of(PHYSFS_getDirSeparator());
	if (std::string::npos != found)
	{
		filename.erase(0, found+1);
	}
	return filename;
}

