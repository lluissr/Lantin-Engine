#ifndef __Config_h__
#define __Config_h__

#include "prettywriter.h"
#include "document.h"
#include "MathGeoLib.h"

enum class ComponentType;

class Config
{
public:
	Config();
	~Config();
	void AddName(const char* name);
	void AddComponentType(const char* name, ComponentType componentType);
	void AddInt(const char* name, int value);
	void AddUnsigned(const char* name, unsigned value);
	void AddFloat(const char* name, float value);
	void AddString(const char* name, const char* value);
	void AddBool(const char* name, bool value);
	void AddFloat3(const char* name, math::float3 value);
	void AddFloat4(const char* name, math::float4 value);
	void AddQuat(const char* name, math::Quat value);

	ComponentType GetComponentType(const char* name, rapidjson::Value& value);
	int GetInt(const char* name, rapidjson::Value& value);
	unsigned GetUnsigned(const char* name, rapidjson::Value& value);
	float GetFloat(const char* name, rapidjson::Value& value);
	const char* GetString(const char* name, rapidjson::Value& value);
	bool GetBool(const char* name, rapidjson::Value& value);
	math::float3 GetFloat3(const char* name, rapidjson::Value& value);
	math::float4 GetFloat4(const char* name, rapidjson::Value& value);
	math::Quat GetQuat(const char* name, rapidjson::Value& value);

	void StartObject();
	void StartObject(const char* name);
	void EndObject();

	void StartArray(const char* name);
	void EndArray();

	void WriteToDisk();
	rapidjson::Document LoadFromDisk();

private:
	rapidjson::StringBuffer* stringBuffer = nullptr;
	rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer = nullptr;
};

#endif

