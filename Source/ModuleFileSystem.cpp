#include "ModuleFileSystem.h"
#include "FileImporter.h"

#include "Application.h"
#include "ModuleResourceManager.h"

#include "Resource.h"

#include "JSON.h"

#include <sys/types.h>
#include <sys/stat.h>

#include "physfs.h"
#include "SDL_timer.h"
#include "SDL_atomic.h"

#include <assert.h>
#include <stack>

#define MONITORIZE_TIME 10000
#define stat _stat

ModuleFileSystem::ModuleFileSystem()
{
	PHYSFS_init(NULL);
	baseDir = PHYSFS_getBaseDir();
	PHYSFS_addToSearchPath(baseDir.c_str(), 1);
	PHYSFS_setWriteDir(baseDir.c_str());

	PHYSFS_mount(SHADERS, nullptr, 1);
	PHYSFS_mount(RESOURCES, nullptr, 1);

	// Assets Folder
	if (!Exists(ASSETS))
	{
		MakeDirectory(ASSETS);
	}
	PHYSFS_mount(ASSETS, nullptr, 1);

	// Library Folder
	if (!Exists(LIBRARY))
	{
		MakeDirectory(LIBRARY);
	}
	PHYSFS_mount(LIBRARY, nullptr, 1);

	if (!Exists(IMPORTED_MATERIALS))
		MakeDirectory(IMPORTED_MATERIALS);
	if (!Exists(MESHES))
		MakeDirectory(MESHES);
	if (!Exists(TEXTURES))
		MakeDirectory(TEXTURES);

}


ModuleFileSystem::~ModuleFileSystem()
{
	PHYSFS_deinit();
}

bool ModuleFileSystem::Init(JSON* config)
{

	return true;
}

bool ModuleFileSystem::Start()
{
	// Check files in Assets and add them to ResManager
	CheckResourcesInFolder(ASSETS);
	if (filesToImport.size() > 0) ImportFiles();

	// Set thread to monitorize Assets folder
	monitor_thread = std::thread(&ModuleFileSystem::Monitorize, this, ASSETS);
	monitor_thread.detach();
	return true;
}

update_status ModuleFileSystem::Update(float dt)
{
	if (filesToImport.size() > 0) ImportFiles();
	return UPDATE_CONTINUE;
}

bool ModuleFileSystem::CleanUp()
{
	monitorize = false;
	while (threadIsWorking)
	{
		SDL_Delay(100);
	}
	return true;
}

unsigned ModuleFileSystem::Load(const char * file, char ** buffer) const
{
	PHYSFS_file* myfile = PHYSFS_openRead(file);
	if (myfile == nullptr)
	{
		LOG("Error: %s %s", file, PHYSFS_getLastError());
		return 0;
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
	int readedObj = PHYSFS_write(myfile, buffer, size, 1);
	if (readedObj < 1)
	{
		LOG("Error: %s %s", file, PHYSFS_getLastError());
		return false;
	}

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
	}

	PHYSFS_freeList(rc);
	return files;
}

void ModuleFileSystem::ListFolderContent(const char * dir, std::vector<std::string>& files, std::vector<std::string>& dirs) const
{
	char** filesList = PHYSFS_enumerateFiles(dir);
	char **i;

	for (i = filesList; *i != nullptr; i++)
	{
		std::string completePath = dir;
		completePath += "/";
		completePath += *i;
		if (IsDirectory(completePath.c_str()))
		{
			dirs.push_back(*i);
		}
		else
		{
			files.push_back(*i);
		}
	}

	PHYSFS_freeList(filesList);
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
	RELEASE(data);
	return true;
}

bool ModuleFileSystem::Copy(const char * source, const char * destination, const char* file) const
{
	char * data = nullptr;
	std::string filepath(source);
	filepath += file;
	unsigned size = Load(filepath.c_str(), &data);
	std::string filedest(destination);
	filedest += file;
	Save(filedest.c_str(), data, size);
	RELEASE_ARRAY(data);

	return true;
}

void ModuleFileSystem::CheckImportedFiles(const char * folder, std::set<std::string>& importedFiles)
{
	importedFiles.clear();
	std::vector<std::string> files = ListFiles(folder);
	for (auto& file : files)
	{
		std::string filefolder(folder);
		filefolder += file;
		if (IsDirectory(filefolder.c_str()))
		{
			CheckImportedFiles((filefolder + "/").c_str(), importedFiles);
		}
		else
		{
			importedFiles.insert(RemoveExtension(file));
		}
	}
}

void ModuleFileSystem::WatchFolder(const char * folder, const std::set<std::string> &textures, const std::set<std::string> &models, const std::set<std::string> &materials)
{
	std::vector<std::string> files;
	std::stack<std::string> watchfolder;
	watchfolder.push(folder);
	std::string current_folder;
	struct stat statFile;
	struct stat statMeta;

	while (!watchfolder.empty())
	{
		current_folder = watchfolder.top();
		watchfolder.pop();

		files = ListFiles(current_folder.c_str());
		for (auto& file : files)
		{
			if (IsDirectory((current_folder + file).c_str()))
			{
				watchfolder.push(current_folder + file + "/");
			}
			else
			{
				stat((current_folder + file).c_str(), &statFile);
				stat((current_folder + file + ".meta").c_str(), &statMeta);
				FILETYPE type = GetFileType(GetExtension(file));
				if (type == FILETYPE::TEXTURE)
				{
					std::set<std::string>::iterator it = textures.find(RemoveExtension(file));
					if (it == textures.end() || statFile.st_mtime > statMeta.st_mtime)
					{
						filesToImport.push_back(std::pair<std::string, std::string>(file, current_folder));
					}
					else
					{
						App->resManager->AddResource(file.c_str(), current_folder.c_str(), TYPE::TEXTURE);
					}
				}
				else if (type == FILETYPE::MODEL)
				{
					//TODO: Get UID from metafile 
					/*unsigned uid = GetMetaUID((current_folder + file + ".meta").c_str());
					std::set<std::string>::iterator it = models.find(std::to_string(uid));
					if (it == models.end() || statFile.st_mtime > statMeta.st_mtime)
					{
						filesToImport.push_back(std::pair<std::string, std::string>(file, current_folder));
					}*/
				}
				else if (type == FILETYPE::MATERIAL)
				{
					std::set<std::string>::iterator it = materials.find(RemoveExtension(file));
					if (it == materials.end() || statFile.st_mtime > statMeta.st_mtime)
					{
						filesToImport.push_back(std::pair<std::string, std::string>(file, current_folder));
					}
					else
					{
						App->resManager->AddResource(file.c_str(), current_folder.c_str(), TYPE::MATERIAL);
					}
				}
			}
		}
	}
	return;
}

void ModuleFileSystem::Monitorize(const char* folder)
{
	while (monitorize)
	{
		threadIsWorking = true;
		LookForNewResourceFiles(folder);
		threadIsWorking = false;
		SDL_Delay(MONITORIZE_TIME);
	}
}

void ModuleFileSystem::CheckResourcesInFolder(const char* folder)
{
	std::set<std::string> importedTextures;
	std::set<std::string> importedModels;
	std::set<std::string> importedMaterials;
	CheckImportedFiles(TEXTURES, importedTextures);
	CheckImportedFiles(SCENES, importedModels);
	CheckImportedFiles(IMPORTED_MATERIALS, importedMaterials);
	WatchFolder(folder, importedTextures, importedModels, importedMaterials);
}

void ModuleFileSystem::LookForNewResourceFiles(const char* folder)
{
	std::vector<std::string> files;
	std::stack<std::string> watchfolder;
	watchfolder.push(folder);
	std::string current_folder;
	struct stat statFile;
	struct stat statMeta;

	while (!watchfolder.empty())
	{
		current_folder = watchfolder.top();
		watchfolder.pop();

		files = ListFiles(current_folder.c_str());
		for (auto& file : files)
		{
			if (IsDirectory((current_folder + file).c_str()))
			{
				watchfolder.push(current_folder + file + "/");
			}
			else
			{
				if (GetExtension(file) == ".meta")
					continue;
				stat((current_folder + file).c_str(), &statFile);
				stat((current_folder + file + ".meta").c_str(), &statMeta);
				std::vector<Resource*> resources = App->resManager->GetResourcesList();
				bool found = false;
				for (std::vector<Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
				{
					if (strcmp((*it)->GetFile(), (current_folder + file).c_str()) == 0)
					{
						found = true;
						break;
					}
				}
				if (!found || statFile.st_mtime > statMeta.st_mtime)
				{
					// TODO: Enable FBX also
					FILETYPE type = GetFileType(GetExtension(file));
					if(type != FILETYPE::MODEL && type != FILETYPE::SCENE)
						filesToImport.push_back(std::pair<std::string, std::string>(file, current_folder));
				}
			}
		}
	}
	return;
}

void ModuleFileSystem::ImportFiles()
{
	for (auto & file : filesToImport)
	{
		importer.ImportAsset(file.first.c_str(), file.second.c_str());
	}
	filesToImport.clear();
}

FILETYPE ModuleFileSystem::GetFileType(std::string extension) const
{
	if (extension == PNG || extension == TIF || extension == JPG || extension == TGA)
	{
		return FILETYPE::TEXTURE;
	}
	if (extension == TEXTUREEXT)
	{
		return FILETYPE::IMPORTED_TEXTURE;
	}
	if (extension == FBXEXTENSION || extension == FBXCAPITAL)
	{
		return FILETYPE::MODEL;
	}
	if (extension == MESHEXTENSION)
	{
		return FILETYPE::IMPORTED_MESH;
	}
	if (extension == JSONEXT)
	{
		return FILETYPE::SCENE;
	}
	if (extension == MATERIALEXT)
	{
		return FILETYPE::MATERIAL;
	}
	return FILETYPE::NONE;
}

std::string ModuleFileSystem::GetExtension(std::string filename) const
{
	std::size_t found = filename.find_last_of(".");
	if (std::string::npos != found)
	{
		filename.erase(0, found);
	}
	return filename;
}

std::string ModuleFileSystem::RemoveExtension(std::string filename) const
{
	std::size_t found = filename.find_last_of(".");
	if (std::string::npos != found)
	{
		filename.erase(found, filename.size());
	}
	return filename;
}

std::string ModuleFileSystem::GetFilename(std::string filename) const
{
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

void ModuleFileSystem::CheckMetaFiles(const char* directory)
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;
	ListFolderContent(directory, files, dirs);

	// [Directories]
	for each (std::string dir in dirs)
	{
		std::string path(directory);
		path += "/" + dir;
		CheckMetaFiles(path.c_str());
	}
	// [Files]
	for each (std::string file in files)
	{
		bool import = false;
		FILETYPE type = GetFileType(GetExtension(file));
		if (type == FILETYPE::SCENE || type == FILETYPE::NONE)
			continue;

		std::string metaFile(directory);
		metaFile += "/" + file + ".meta";
		if (Exists((metaFile).c_str()))
		{
			if (CheckImportedFile(file.c_str()))
			{
				LOG("Exists, %s", file.c_str());
				std::string path(directory);
				App->resManager->AddResource(file.c_str(), (path + "/").c_str(), (type == FILETYPE::TEXTURE) ? TYPE::TEXTURE : TYPE::MESH);
			}
			else
			{
				import = true;
			}
		}
		else
		{
			import = true;
		}

		if (import)
		{
			std::string path(directory);
			filesToImport.push_back(std::pair<std::string, std::string>(file, path + "/"));
		}
	}
}

bool ModuleFileSystem::CheckImportedFile(const char* file)
{
	std::string extension = GetExtension(file);
	std::set<std::string> importedFiles;
	switch (GetFileType(extension))
	{
	case FILETYPE::TEXTURE:
		// Look in imported textures folder 
		CheckImportedFiles(TEXTURES, importedFiles);
		for (std::set<std::string>::const_iterator it = importedFiles.begin(); it != importedFiles.end(); ++it)
		{
			if (strcmp((*it).c_str(), GetFilename(file).c_str()) == 0)
			{
				return true;
			}
		}
		break;
	case FILETYPE::MODEL:
		// Look in imported textures folder 
		CheckImportedFiles(MESHES, importedFiles);
		// TODO: Check meta file UID and compare it with the files imported
		/*for (std::set<std::string>::const_iterator it = importedFiles.begin(); it != importedFiles.end(); ++it)
		{
			if (strcmp((*it).c_str(), GetFilename(file).c_str()) == 0)
			{
				return true;
			}
		}*/
		return true;
		break;
	default:
		break;
	}

	return true;
}