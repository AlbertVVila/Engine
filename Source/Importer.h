#ifndef __Importer_h__
#define __Importer_h__

#include <string>
class Importer
{
public:
	Importer();
	virtual ~Importer();

	virtual bool Import(const char* file, const char* path, std::string& output_file)=0;
};

#endif __Importer_h__

