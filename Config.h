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

	void AddName(const char* name) const ;
	void AddComponentType(const char* name, ComponentType componentType) const;
	void AddInt(const char* name, int value) const;
	void AddUnsigned(const char* name, unsigned value) const;
	void AddFloat(const char* name, float value) const;
	void AddString(const char* name, const char* value) const;
	void AddBool(const char* name, bool value) const;
	void AddFloat3(const char* name, math::float3 value) const;
	void AddFloat4(const char* name, math::float4 value) const;
	void AddQuat(const char* name, math::Quat value) const;

	ComponentType GetComponentType(const char* name, rapidjson::Value& value) const;
	int GetInt(const char* name, rapidjson::Value& value) const;
	unsigned GetUnsigned(const char* name, rapidjson::Value& value) const;
	float GetFloat(const char* name, rapidjson::Value& value) const;
	const char* GetString(const char* name, rapidjson::Value& value) const;
	bool GetBool(const char* name, rapidjson::Value& value) const;
	math::float3 GetFloat3(const char* name, rapidjson::Value& value) const;
	math::float4 GetFloat4(const char* name, rapidjson::Value& value) const;
	math::Quat GetQuat(const char* name, rapidjson::Value& value) const;

	void StartObject() const;
	void StartObject(const char* name) const;
	void EndObject() const;

	void StartArray(const char* name) const;
	void EndArray() const;

	void WriteToDisk(const char* name);
	rapidjson::Document LoadFromDisk(const char* name);

private:
	rapidjson::StringBuffer* stringBuffer = nullptr;
	rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer = nullptr;
};

#endif

