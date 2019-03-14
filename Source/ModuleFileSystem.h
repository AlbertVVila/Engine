#ifndef __ModuleFileSystem_h__
#define __ModuleFileSystem_h__

#include "Module.h"
#include "Timer.h"
#include <vector>
#include <list>
#include <set>
#include <thread>
#include "FileImporter.h"

#define ASSETS "Assets/"
#define LIBRARY "Library/"
#define RESOURCES "Resources/"
#define IMPORTED_RESOURCES RESOURCES "Imported/"
#define MATERIALS LIBRARY "Materials/"
#define MESHES LIBRARY "Meshes/"
#define TEXTURES LIBRARY "Textures/"
#define SCENES ASSETS "Scenes/"
#define SHADERS "Shaders/"
#define VERTEXSHADERS SHADERS "VertexShaders/"
#define FRAGSHADERS SHADERS "FragmentShaders/"

#define FBXEXTENSION ".fbx"
#define FBXCAPITAL ".FBX"
#define MESHEXTENSION ".m3sh"
#define PNG ".png"
#define TIF	".tif"
#define JPG	".jpg"
#define TEXTUREEXT ".dds"
#define JSONEXT ".json"

#define CHECKERS "checkersTexture"
#define NOCAMERA "nocamera"

enum class FILETYPE
{
	TEXTURE,
	IMPORTED_TEXTURE,
	MODEL,
	IMPORTED_MESH,
	SCENE,
	NONE
};

class ModuleFileSystem :
	public Module
{
public:
	ModuleFileSystem();
	~ModuleFileSystem();

	bool Init(JSON* config) override;
	bool Start() override;
	update_status Update(float dt) override;
	bool CleanUp() override;

	unsigned Load(const char * file, char ** buffer) const;
	bool Save(const char* file, const char* buffer, unsigned size) const;
	bool Remove(const char* file) const;
	bool Exists(const char* file) const;
	unsigned Size(const char* file) const;
	bool MakeDirectory(const char* directory) const;
	bool IsDirectory(const char* file) const;
	std::vector<std::string> ListFiles(const char * dir, bool extension=true) const;
	void ListFolderContent(const char * dir, std::vector<std::string>& files, std::vector<std::string>& dirs) const; // Saves all files found on dir on files vector and all directories on dirs vector
	bool CopyFromOutsideFS(const char* source, const char* destination) const;
	bool Copy(const char* source, const char* destination, const char* file) const;

	std::string GetExtension(std::string file) const;
	std::string RemoveExtension(std::string file) const;
	std::string GetFilename(std::string file) const;

private:
	void CheckImportedFiles(const char* folder, std::set<std::string>& importedFiles);
	void WatchFolder(const char* folder, const std::set<std::string> &textures, const std::set<std::string> &models);
	void Monitorize(const char* folder);
	void CheckResourcesInFolder(const char* folder); // Checks the resource files located in the folder to import and add them to Resources list (if not already)
	void ImportFiles();
	FILETYPE GetFileType(std::string) const;

	// Resource Management
	void CheckMetaFiles(const char* directory);
	bool CheckImportedFile(const char* file);

public:
	std::string baseDir = "";
	FileImporter importer;

private:
	bool monitorize = true;
	bool threadIsWorking = false;
	Timer importTimer;
	std::list<std::pair<std::string, std::string>> filesToImport; //File and folder
	std::thread monitor_thread;
};

#endif __ModuleFileSystem_h__