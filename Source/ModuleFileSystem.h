#ifndef __ModuleFileSystem_h__
#define __ModuleFileSystem_h__

#include "Module.h"
#include "Timer.h"
#include <list>
#include <set>

#define ASSETS "Assets/"
#define LIBRARY "Library/"
#define MESHES LIBRARY "Meshes/"
#define MATERIALS LIBRARY "Materials/"
#define SCENES LIBRARY "Scenes/"
#define SKYBOX "Skybox/"

#define FBXEXTENSION ".fbx"
#define MESHEXTENSION ".m3sh"
#define MATERIALEXTENSION ".dds"
#define JSONEXT ".json"

#define CHECKERS "checkersTexture"
#define NOCAMERA "nocamera"
class ModuleFileSystem :
	public Module
{
public:
	ModuleFileSystem();
	~ModuleFileSystem();

	bool Init() override;
	bool Start() override;
	update_status Update() override;

	unsigned Load(const char * file, char ** buffer) const;
	bool Save(const char* file, const char* buffer, unsigned size) const;
	bool Remove(const char* file) const;
	bool Exists(const char* file) const;
	unsigned Size(const char* file) const;
	bool MakeDirectory(const char* directory);
	bool IsDirectory(const char* file) const;
	std::list<std::string> ListFiles(const char * dir) const;
	bool CopyFromOutsideFS(const char* source, const char* destination);
	bool Copy(const char* source, const char* destination, const char* file);

	void CheckImportedFiles(const char * folder);

	void WatchFolder(const char* folder);

	std::string GetExtension(const char * file) const;

	std::string RemoveExtension(const char * file) const;
	std::string GetFilename(const char *file) const;
private:
	float watchThreshold = 1000.f;
	Timer importTimer;
	std::set<std::string> importedFiles;
};

#endif __ModuleFileSystem_h__