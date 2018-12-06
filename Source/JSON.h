#ifndef __JSON_h__
#define __JSON_h__

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include <string>
#include <vector>

struct JSON_value
{
public:
	JSON_value(rapidjson::Document::AllocatorType* allocator, rapidjson::Type type = rapidjson::kObjectType);
	JSON_value * CreateValue(rapidjson::Type type = rapidjson::kObjectType);

	void AddInt(const char* name, int value);
	void AddUint(const char * name, unsigned value);
	void AddFloat(const char * name, float value);
	void AddString(const char * name, const char* value);
	void AddValue(const char * name, JSON_value *value);

public:
	rapidjson::Value *rapidjsonValue = nullptr;
	rapidjson::Document::AllocatorType* allocator = nullptr;
	std::vector<JSON_value*> valuesAllocated;
};

class JSON
{
public:
	JSON();
	~JSON();

	JSON_value* CreateValue(rapidjson::Type type = rapidjson::kObjectType);
	rapidjson::Value* Array(const char* name);
	void AddValue(const char * name, JSON_value * value);
	std::string ToString();
	unsigned Size();

private:
	rapidjson::Document *document = nullptr;
	rapidjson::Document::AllocatorType* allocator = nullptr;
	rapidjson::StringBuffer *jsonBuffer = nullptr;
	std::vector<JSON_value*> valuesAllocated;
};

#endif __JSON_h__
