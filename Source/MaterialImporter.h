#ifndef __MaterialImporter_h__
#define __MaterialImporter_h__

#include "Importer.h"
class MaterialImporter :
	public Importer
{
public:
	MaterialImporter();
	~MaterialImporter();

	bool Import(const char* file, const char* path, std::string& output_file) override;
};

#endif __MaterialImporter_h__

