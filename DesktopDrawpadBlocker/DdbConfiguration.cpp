#include "DdbConfiguration.h"

#include "IdtText.h"
#include "JsonCpp/reader.h"
#include "JsonCpp/value.h"
#include "JsonCpp/writer.h"

#include <fstream>

SetListStruct setList;

bool ConfigurationChange()
{
	if (_waccess((StringToWstring(globalPath) + L"interaction_configuration.json").c_str(), 4) == -1) return false;

	bool ret = false;

	Json::Reader reader;
	Json::Value root;

	ifstream readjson;
	readjson.imbue(locale("zh_CN.UTF8"));
	readjson.open(WstringToString(StringToWstring(globalPath) + L"interaction_configuration.json").c_str());

	if (reader.parse(readjson, root))
	{
		if (root.isMember("~ConfigurationChange") && root["~ConfigurationChange"].isBool())
		{
			ret = root["~ConfigurationChange"].asBool();
		}
	}

	readjson.close();

	return ret;
}
bool CloseSoftware()
{
	if (_waccess((StringToWstring(globalPath) + L"interaction_configuration.json").c_str(), 4) == -1) return false;

	bool ret = true;

	Json::Reader reader;
	Json::Value root;

	ifstream readjson;
	readjson.imbue(locale("zh_CN.UTF8"));
	readjson.open(WstringToString(StringToWstring(globalPath) + L"interaction_configuration.json").c_str());

	if (reader.parse(readjson, root))
	{
		if (root.isMember("~KeepOpen") && root["~KeepOpen"].isBool())
		{
			ret = !root["~KeepOpen"].asBool();
		}
	}

	readjson.close();

	return ret;
}

bool ReadSetting()
{
	if (_waccess((StringToWstring(globalPath) + L"interaction_configuration.json").c_str(), 4) == -1) return false;

	Json::Reader reader;
	Json::Value root;

	ifstream readjson;
	readjson.imbue(locale("zh_CN.UTF8"));
	readjson.open(WstringToString(StringToWstring(globalPath) + L"interaction_configuration.json").c_str());

	if (reader.parse(readjson, root))
	{
		if (root.isMember("SleepTime") && root["SleepTime"].isInt())
		{
			setList.sleepTime = root["SleepTime"].asInt();
		}

		if (root.isMember("Mode") && root["Mode"].isObject())
		{
			if (root["Mode"].isMember("Mode") && root["Mode"]["Mode"].isInt())
			{
				setList.mode = root["Mode"]["Mode"].asInt();
			}
			if (root["Mode"].isMember("HostPath") && root["Mode"]["HostPath"].isString())
			{
				setList.hostPath = StringToWstring(ConvertToGbk(root["Mode"]["HostPath"].asString()));
			}
			if (root["Mode"].isMember("RestartHost") && root["Mode"]["RestartHost"].isBool())
			{
				setList.restartHost = root["Mode"]["restartHost"].asBool();
			}
		}

		if (root.isMember("Intercept") && root["Intercept"].isObject())
		{
			if (root["Intercept"].isMember("AiClassFloating") && root["Intercept"]["AiClassFloating"].isBool())
			{
				setList.InterceptWindow[0] = root["Intercept"]["AiClassFloating"].asBool();
			}
			if (root["Intercept"].isMember("SeewoWhiteboardFloating") && root["Intercept"]["SeewoWhiteboardFloating"].isBool())
			{
				setList.InterceptWindow[1] = root["Intercept"]["SeewoWhiteboardFloating"].asBool();
			}
			if (root["Intercept"].isMember("SeewoPincoFloating") && root["Intercept"]["SeewoPincoFloating"].isBool())
			{
				setList.InterceptWindow[2] = setList.InterceptWindow[3] = root["Intercept"]["SeewoPincoFloating"].asBool();
			}

			if (root["Intercept"].isMember("SeewoPPTFloating") && root["Intercept"]["SeewoPPTFloating"].isBool())
			{
				setList.InterceptWindow[4] = root["Intercept"]["SeewoPPTFloating"].asBool();
			}
		}
	}

	readjson.close();

	return true;
}
bool WriteSetting(bool close)
{
	Json::StyledWriter outjson;
	Json::Value root;

	root["Edition"] = Json::Value(editionDate);

	root["SleepTime"] = Json::Value(setList.sleepTime);

	root["Mode"]["Mode"] = Json::Value(setList.mode);
	root["Mode"]["HostPath"] = Json::Value(ConvertToUtf8(WstringToString(setList.hostPath)));
	root["Mode"]["RestartHost"] = Json::Value(setList.restartHost);

	root["Intercept"]["AiClassFloating"] = Json::Value(setList.InterceptWindow[0]);
	root["Intercept"]["SeewoWhiteboardFloating"] = Json::Value(setList.InterceptWindow[1]);
	root["Intercept"]["SeewoPincoFloating"] = Json::Value(setList.InterceptWindow[2]);
	root["Intercept"]["SeewoPPTFloating"] = Json::Value(setList.InterceptWindow[4]);

	root["~ConfigurationChange"] = Json::Value(false);
	root["~KeepOpen"] = Json::Value(!close);

	ofstream writejson;
	writejson.imbue(locale("zh_CN.UTF8"));
	writejson.open(WstringToString(StringToWstring(globalPath) + L"interaction_configuration.json").c_str());
	writejson << outjson.write(root);
	writejson.close();

	return true;
}