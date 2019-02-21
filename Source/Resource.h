#ifndef __Resource_h__
#define __Resource_h__

#include <string>

struct JSON_value;

enum class TYPE
{
	TEXTURE,
	MESH,
	AUDIO,
	SCENE,
	BONE,
	ANIMATION,
	UNKNOWN
};

class Resource
{
public:
	Resource(unsigned uid, TYPE type);
	virtual ~Resource();

	inline TYPE GetType() const;
	unsigned GetUID() const;
	const char* GetFile() const;
	void SetFile(const char* newFile);
	const char* GetExportedFile() const;
	void SetExportedFile(const char* newExportedFile);
	inline bool IsLoadedToMemory() const;
	bool LoadToMemory();
	unsigned CountReferences() const;
	void DecreaseReferences();
	
	virtual void Save(JSON_value &config) const;
	virtual void Load(const JSON_value &config);
	virtual bool LoadInMemory() = 0;

protected:
	unsigned UID = 0u;
	std::string file;
	std::string exportedFile;
	unsigned loaded = 0;

	TYPE type = TYPE::UNKNOWN;
};

#endif __Resource_h__