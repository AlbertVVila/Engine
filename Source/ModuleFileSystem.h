#ifndef __ModuleFileSystem_h__
#define __ModuleFileSystem_h__

#include "Module.h"
class ModuleFileSystem :
	public Module
{
public:
	ModuleFileSystem();
	~ModuleFileSystem();

	unsigned int Load(const char* path, const char* file, char** buffer) const;
	unsigned int Save(const char* file, const char* buffer, unsigned size) const;
	bool Remove(const char* file) const;
	bool Exists(const char* file) const;
	bool MakeDirectory(const char* directory);
	bool IsDirectory(const char* file) const;
	const char * GetExtension(const char * file) const;
	bool Copy(const char* source, const char* destination);
};

#endif __ModuleFileSystem_h__