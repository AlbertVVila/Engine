#ifndef __JSON_h__
#define __JSON_h__

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include <string>
#include <vector>
#include "Math/float3.h"
#include "Math/float4.h"
#include "Math/Quat.h"

struct Texture;

class JSON_value
{
public:
	JSON_value(rapidjson::Document::AllocatorType* allocator, rapidjson::Type type = rapidjson::kObjectType);
	~JSON_value();

	JSON_value * CreateValue(rapidjson::Type type = rapidjson::kObjectType);

	void AddInt(const char* name, int value);
	void AddUint(const char * name, unsigned value);
	void AddFloat(const char * name, float value);
	void AddFloat3(const char * name, float3 value);
	void AddFloat4(const char * name, float4 value);
	void AddQuat(const char * name, Quat value);
	void AddString(const char * name, const char* value);
	void AddValue(const char * name, JSON_value *value);

	int GetInt(const char* name) const;
	unsigned GetUint(const char * name) const;
	float GetFloat(const char * namee) const;
	float3 GetFloat3(const char * name) const;
	float3 GetColor3(const char * name) const;
	float4 GetFloat4(const char * name) const;
	float4 GetColor4(const char * name) const;
	Quat GetQuat(const char * name) const;
	const char* GetString(const char * name) const;

	JSON_value* GetValue(unsigned index); //Allocates value
	JSON_value * GetValue(const char * name); //Allocates value
	unsigned Size() const;

public:
	rapidjson::Value *rapidjsonValue = nullptr;
	rapidjson::Document::AllocatorType* allocator = nullptr;
private:
	std::vector<JSON_value*> valuesAllocated;
};

class JSON
{
public:
	JSON();
	JSON(const char * data);
	~JSON();

	JSON_value* CreateValue(rapidjson::Type type = rapidjson::kObjectType);
	void AddValue(const char * name, JSON_value * value);
	JSON_value * GetValue(const char * name);  //Allocates value
	std::string ToString() const;
	unsigned Size();

private:
	rapidjson::Document *document = nullptr;
	rapidjson::Document::AllocatorType* allocator = nullptr;
	rapidjson::StringBuffer *jsonBuffer = nullptr;
	std::vector<JSON_value*> valuesAllocated;
};

#endif __JSON_h__
