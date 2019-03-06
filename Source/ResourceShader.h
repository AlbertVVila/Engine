#ifndef __ResourceShader_h__
#define __ResourceShader_h__

#include "Resource.h"

#include <string>

class ResourceShader :
	public Resource
{
public:
	ResourceShader(unsigned uid);
	~ResourceShader();

	bool LoadInMemory() override;
	void DeleteFromMemory() override;

private:
	unsigned CreateVertexShader();
	unsigned CreateFragmentShader();
	void ShaderLog(unsigned int shader, char * type) const;

	virtual void Save(JSON_value& config) const override;
	virtual void Load(const JSON_value& config) override;

public:
	unsigned id = 0u;
};

#endif __ResourceShader_h__