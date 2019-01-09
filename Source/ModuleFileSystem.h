#ifndef __ModuleFileSystem_h__
#define __ModuleFileSystem_h__

#include "Module.h"
#include "Timer.h"
#include <vector>
#include <set>

#define ASSETS "Assets/"
#define LIBRARY "Library/"
#define MATERIALS LIBRARY "Materials/"
#define MESHES LIBRARY "Meshes/"
#define TEXTURES LIBRARY "Textures/"
#define SCENES LIBRARY "Scenes/"
#define SHADERS "Shaders/"
#define VERTEXSHADERS SHADERS "VertexShaders/"
#define FRAGSHADERS SHADERS "FragmentShaders/"
#define SKYBOX "Skybox/"

#define FBXEXTENSION ".fbx"
#define MESHEXTENSION ".m3sh"
#define TEXTUREEXT ".dds"
#define JSONEXT ".json"

#define CHECKERS "checkersTexture"
#define NOCAMERA "nocamera"

class ModuleFileSystem :
	public Module
{
public:
	ModuleFileSystem();
	~ModuleFileSystem();

	bool Start() override;
	update_status Update() override;

	unsigned Load(const char * file, char ** buffer) const;
	bool Save(const char* file, const char* buffer, unsigned size) const;
	bool Remove(const char* file) const;
	bool Exists(const char* file) const;
	unsigned Size(const char* file) const;
	bool MakeDirectory(const char* directory) const;
	bool IsDirectory(const char* file) const;
	std::vector<std::string> ListFiles(const char * dir, bool extension=true) const;
	bool CopyFromOutsideFS(const char* source, const char* destination) const;
	bool Copy(const char* source, const char* destination, const char* file) const;

	std::string GetExtension(const char * file) const;
	std::string RemoveExtension(const char * file) const;
	std::string GetFilename(const char *file) const;

private:
	void CheckImportedFiles(const char * folder);
	void WatchFolder(const char* folder);

private:
	float watchThreshold = 1000.f;
	Timer importTimer;
	std::set<std::string> importedFiles;
};

#endif __ModuleFileSystem_h__