#include "Globals.h"
#include "JSON.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"


JSON_value::JSON_value(rapidjson::Document::AllocatorType * allocator, rapidjson::Type type): allocator(allocator)
{
		rapidjsonValue = new rapidjson::Value(type);
}; //TODO: release Memory

JSON_value* JSON_value::CreateValue(rapidjson::Type type)
{
	JSON_value* ret = new JSON_value(allocator, type);
	valuesAllocated.push_back(ret);
	return ret;
}

void JSON_value::AddInt(const char * name, int value)
{
	std::string myname(name);
	rapidjson::Value key(myname.c_str(), myname.size(), *allocator);
	rapidjsonValue->AddMember(key, value, *allocator);
}

void JSON_value::AddUint(const char * name, unsigned value)
{
	std::string myname(name);
	rapidjson::Value key(myname.c_str(), myname.size(), *allocator);
	rapidjsonValue->AddMember(key, value, *allocator);
}

void JSON_value::AddFloat(const char * name, float value)
{
	std::string myname(name);
	rapidjson::Value key(myname.c_str(), myname.size(), *allocator);
	rapidjsonValue->AddMember(key, value, *allocator);
}

void JSON_value::AddFloat3(const char * name, float3 value)
{
	std::string myname(name);
	rapidjson::Value key(myname.c_str(), myname.size(), *allocator);

	rapidjson::Value float3(rapidjson::kArrayType);
	float3.PushBack(value.x, *allocator);
	float3.PushBack(value.y, *allocator);
	float3.PushBack(value.z, *allocator);

	rapidjsonValue->AddMember(key, float3, *allocator);
}

void JSON_value::AddFloat4(const char * name, float4 value)
{
	std::string myname(name);
	rapidjson::Value key(myname.c_str(), myname.size(), *allocator);

	rapidjson::Value float4(rapidjson::kArrayType);
	float4.PushBack(value.x, *allocator);
	float4.PushBack(value.y, *allocator);
	float4.PushBack(value.z, *allocator);
	float4.PushBack(value.w, *allocator);

	rapidjsonValue->AddMember(key, float4, *allocator);
}

void JSON_value::AddQuat(const char * name, Quat value)
{
	std::string myname(name);
	rapidjson::Value key(myname.c_str(), myname.size(), *allocator);

	rapidjson::Value quat(rapidjson::kArrayType);
	quat.PushBack(value.x, *allocator);
	quat.PushBack(value.y, *allocator);
	quat.PushBack(value.z, *allocator);
	quat.PushBack(value.w, *allocator);

	rapidjsonValue->AddMember(key, quat, *allocator);
}

void JSON_value::AddString(const char * name, const char* value)
{
	std::string myname(name);
	rapidjson::Value key(myname.c_str(), myname.size(), *allocator);

	std::string myvalue(value);
	rapidjson::Value rvalue(myvalue.c_str(), myvalue.size(), *allocator);

	rapidjsonValue->AddMember(key, rvalue, *allocator);
}

void JSON_value::AddValue(const char* name, JSON_value *value)
{
	switch (this->rapidjsonValue->GetType())
	{
	case rapidjson::kObjectType:
	{
		std::string myname(name);
		rapidjson::Value key(myname.c_str(), myname.size(), *allocator);
		this->rapidjsonValue->AddMember(key, *value->rapidjsonValue, *allocator);
	}
	break;

	case rapidjson::kArrayType:
		this->rapidjsonValue->PushBack(*value->rapidjsonValue, *allocator);
		break;
	}
}


JSON::JSON()
{
	document = new rapidjson::Document();
	document->SetObject();
	allocator = &document->GetAllocator();
	jsonBuffer = new rapidjson::StringBuffer();
}

JSON::JSON(const char *data)
{
	document = new rapidjson::Document();
	document->Parse(data);
	document->SetObject();
	allocator = &document->GetAllocator();
	jsonBuffer = new rapidjson::StringBuffer();
}

JSON::~JSON()
{
	RELEASE(document);
}


JSON_value * JSON::CreateValue(rapidjson::Type type)
{
	JSON_value* ret = new JSON_value(allocator, type);
	valuesAllocated.push_back(ret);
	return ret;
}

void JSON::AddValue(const char* name, JSON_value* value)
{
	std::string myname(name);
	rapidjson::Value key(myname.c_str(), myname.size(), *allocator);
	document->AddMember(key, *value->rapidjsonValue, *allocator);
}

JSON_value * JSON::GetValue(const char* name)
{
	if (document->HasMember(name))
	{
		JSON_value * val = new JSON_value(allocator);
		val->rapidjsonValue->CopyFrom(document->operator[](name), *allocator);
		valuesAllocated.push_back(val);
		return val;
	}
	return nullptr;
}


std::string JSON::ToString()
{
	jsonBuffer->Clear();
	rapidjson::Writer<rapidjson::StringBuffer> writer(*jsonBuffer);
	document->Accept(writer);

	return jsonBuffer->GetString();
}

unsigned JSON::Size()
{
	jsonBuffer->Clear();
	rapidjson::Writer<rapidjson::StringBuffer> writer(*jsonBuffer);
	document->Accept(writer);
	return jsonBuffer->GetSize();
}



