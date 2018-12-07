#ifndef __ModuleFileSystem_h__
#define __ModuleFileSystem_h__

#include "Module.h"
#include "Timer.h"
#include <list>
#include <set>

#define ASSETS "Assets/"
#define LIBRARY "Library/"
#define MESHES "Meshes/"
#define MATERIALS "Materials/"
#define SKYBOX "Skybox/"
#define DATADIR "Data"

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

	void WatchFolder(const char* folder);

	std::string GetExtension(const char * file) const;

private:
	float watchThreshold = 1000.f;
	Timer importTimer;
	std::set<const char*> importedFiles;
};

#endif __ModuleFileSystem_h__