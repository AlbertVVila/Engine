#include "Resource.h"

#include "JSON.h"

Resource::Resource(unsigned uid, TYPE type): UID(uid), type(type)
{
}

Resource::~Resource()
{
}

inline TYPE Resource::GetType() const
{
	return type;
}

inline unsigned Resource::GetUID() const
{
	return UID;
}

inline const char* Resource::GetFile() const
{
	return file.c_str();
}

inline const char* Resource::GetExportedFile() const
{
	return exportedFile.c_str();
}

void Resource::Save(JSON_value &config) const
{
	JSON_value *resource = config.CreateValue();
	resource->AddUint("UID", UID);
	resource->AddString("File", file.c_str());
	resource->AddString("ExportedFile", exportedFile.c_str());

	resource->AddUint("Type", (unsigned)type);
}

void Resource::Load(const JSON_value &config)
{
	UID = config.GetUint("UID");
	file = config.GetString("File");
	exportedFile = config.GetString("ExportedFile");

	type = (TYPE)config.GetUint("Type");
}