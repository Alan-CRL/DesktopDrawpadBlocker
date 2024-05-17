#include "DdbConfiguration.h"

#include "IdtText.h"
#include "JsonCpp/json.h"

#include <fstream>

DdbSetListStruct ddbSetList;

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

bool DdbReadSetting()
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
			ddbSetList.sleepTime = root["SleepTime"].asInt();
		}

		if (root.isMember("Mode") && root["Mode"].isObject())
		{
			if (root["Mode"].isMember("Mode") && root["Mode"]["Mode"].isInt())
			{
				ddbSetList.mode = root["Mode"]["Mode"].asInt();
			}
			if (root["Mode"].isMember("HostPath") && root["Mode"]["HostPath"].isString())
			{
				ddbSetList.hostPath = StringToWstring(root["Mode"]["HostPath"].asString());
			}
			if (root["Mode"].isMember("RestartHost") && root["Mode"]["RestartHost"].isBool())
			{
				ddbSetList.restartHost = root["Mode"]["RestartHost"].asBool();
			}
		}

		if (root.isMember("Intercept") && root["Intercept"].isObject())
		{
			if (root["Intercept"].isMember("AiClassFloating") && root["Intercept"]["AiClassFloating"].isBool())
			{
				ddbSetList.InterceptWindow[0] = root["Intercept"]["AiClassFloating"].asBool();
			}
			if (root["Intercept"].isMember("SeewoWhiteboardFloating") && root["Intercept"]["SeewoWhiteboardFloating"].isBool())
			{
				ddbSetList.InterceptWindow[1] = root["Intercept"]["SeewoWhiteboardFloating"].asBool();
			}
			if (root["Intercept"].isMember("SeewoPincoFloating") && root["Intercept"]["SeewoPincoFloating"].isBool())
			{
				ddbSetList.InterceptWindow[2] = ddbSetList.InterceptWindow[3] = root["Intercept"]["SeewoPincoFloating"].asBool();
			}

			if (root["Intercept"].isMember("SeewoPPTFloating") && root["Intercept"]["SeewoPPTFloating"].isBool())
			{
				ddbSetList.InterceptWindow[4] = root["Intercept"]["SeewoPPTFloating"].asBool();
			}
		}
	}

	readjson.close();

	return true;
}
bool DdbWriteSetting(bool close)
{
	Json::Value root;

	root["Edition"] = Json::Value(editionDate);

	root["SleepTime"] = Json::Value(ddbSetList.sleepTime);

	root["Mode"]["Mode"] = Json::Value(ddbSetList.mode);
	root["Mode"]["HostPath"] = Json::Value(WstringToString(ddbSetList.hostPath));
	root["Mode"]["RestartHost"] = Json::Value(ddbSetList.restartHost);

	root["Intercept"]["AiClassFloating"] = Json::Value(ddbSetList.InterceptWindow[0]);
	root["Intercept"]["SeewoWhiteboardFloating"] = Json::Value(ddbSetList.InterceptWindow[1]);
	root["Intercept"]["SeewoPincoFloating"] = Json::Value(ddbSetList.InterceptWindow[2]);
	root["Intercept"]["SeewoPPTFloating"] = Json::Value(ddbSetList.InterceptWindow[4]);

	root["~ConfigurationChange"] = Json::Value(false);
	root["~KeepOpen"] = Json::Value(!close);

	Json::StreamWriterBuilder outjson;
	outjson.settings_["emitUTF8"] = true;
	std::unique_ptr<Json::StreamWriter> writer(outjson.newStreamWriter());
	ofstream writejson;
	writejson.imbue(locale("zh_CN.UTF8"));
	writejson.open(WstringToString(StringToWstring(globalPath) + L"interaction_configuration.json").c_str());
	writer->write(root, &writejson);
	writejson.close();

	return true;
}