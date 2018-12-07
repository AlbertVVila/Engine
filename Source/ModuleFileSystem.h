#ifndef __ModuleFileSystem_h__
#define __ModuleFileSystem_h__

#include "Module.h"
#include <list>

class ModuleFileSystem :
	public Module
{
public:
	ModuleFileSystem();
	~ModuleFileSystem();

	bool Init();

	unsigned Load(const char * file, char ** buffer) const;
	bool Save(const char* file, const char* buffer, unsigned size) const;
	bool Remove(const char* file) const;
	bool Exists(const char* file) const;
	unsigned Size(const char* file) const;
	bool MakeDirectory(const char* directory);
	bool IsDirectory(const char* file) const;
	std::list<std::string> ListFiles(const char * dir) const;
	bool Copy(const char* source, const char* destination);
};

#endif __ModuleFileSystem_h__