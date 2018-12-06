#include "Globals.h"
#include "JSON.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"


JSON_value::JSON_value(rapidjson::Document::AllocatorType * allocator, rapidjson::Type type): allocator(allocator)
{
		rapidjsonValue = new rapidjson::Value(type);
};

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

rapidjson::Value* JSON::Array(const char* name)
{
	rapidjson::Value *myarray = new rapidjson::Value(rapidjson::kArrayType);
	rapidjson::StringBuffer s;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);

	std::string str(name);
	rapidjson::Value key(str.c_str(), str.size(), document->GetAllocator());

	document->AddMember(key, *myarray, document->GetAllocator());

	return myarray;
}

void JSON::AddValue(const char* name, JSON_value* value)
{
	std::string myname(name);
	rapidjson::Value key(myname.c_str(), myname.size(), *allocator);
	document->AddMember(key, *value->rapidjsonValue, *allocator);
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



