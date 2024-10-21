#include "DdbConfiguration.h"

#include "IdtText.h"
#include "JsonCpp/json.h"

#include <fstream>

DdbSetListStruct ddbSetList;

bool ConfigurationChange(HANDLE* hFile)
{
	bool ret = false;

	LARGE_INTEGER fileSize;
	if (!GetFileSizeEx(*hFile, &fileSize)) return ret;

	DWORD dwSize = static_cast<DWORD>(fileSize.QuadPart);
	string jsonContent(dwSize, '\0');

	DWORD bytesRead = 0;
	if (SetFilePointer(*hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) return ret;
	if (!ReadFile(*hFile, &jsonContent[0], dwSize, &bytesRead, NULL) || bytesRead != dwSize) return ret;
	if (jsonContent.compare(0, 3, "\xEF\xBB\xBF") == 0) jsonContent = jsonContent.substr(3);

	istringstream jsonContentStream(jsonContent);
	Json::CharReaderBuilder readerBuilder;
	Json::Value updateVal;
	string jsonErr;

	if (Json::parseFromStream(readerBuilder, jsonContentStream, &updateVal, &jsonErr))
	{
		if (updateVal.isMember("~ConfigurationChange") && updateVal["~ConfigurationChange"].isBool())
		{
			ret = updateVal["~ConfigurationChange"].asBool();
		}
	}

	return ret;
}
bool CloseSoftware(HANDLE* hFile)
{
	bool ret = true;

	LARGE_INTEGER fileSize;
	if (!GetFileSizeEx(*hFile, &fileSize)) return ret;

	DWORD dwSize = static_cast<DWORD>(fileSize.QuadPart);
	string jsonContent(dwSize, '\0');

	DWORD bytesRead = 0;
	if (SetFilePointer(*hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) return ret;
	if (!ReadFile(*hFile, &jsonContent[0], dwSize, &bytesRead, NULL) || bytesRead != dwSize) return ret;
	if (jsonContent.compare(0, 3, "\xEF\xBB\xBF") == 0) jsonContent = jsonContent.substr(3);

	istringstream jsonContentStream(jsonContent);
	Json::CharReaderBuilder readerBuilder;
	Json::Value updateVal;
	string jsonErr;

	if (Json::parseFromStream(readerBuilder, jsonContentStream, &updateVal, &jsonErr))
	{
		if (updateVal.isMember("~KeepOpen") && updateVal["~KeepOpen"].isBool())
		{
			ret = !updateVal["~KeepOpen"].asBool();
		}
	}

	return ret;
}

bool DdbReadSetting(HANDLE* hFile)
{
	LARGE_INTEGER fileSize;
	if (!GetFileSizeEx(*hFile, &fileSize)) return false;

	DWORD dwSize = static_cast<DWORD>(fileSize.QuadPart);
	string jsonContent(dwSize, '\0');

	DWORD bytesRead = 0;
	if (SetFilePointer(*hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) return false;
	if (!ReadFile(*hFile, &jsonContent[0], dwSize, &bytesRead, NULL) || bytesRead != dwSize) return false;
	if (jsonContent.compare(0, 3, "\xEF\xBB\xBF") == 0) jsonContent = jsonContent.substr(3);

	istringstream jsonContentStream(jsonContent);
	Json::CharReaderBuilder readerBuilder;
	Json::Value updateVal;
	string jsonErr;

	if (Json::parseFromStream(readerBuilder, jsonContentStream, &updateVal, &jsonErr))
	{
		if (updateVal.isMember("SleepTime") && updateVal["SleepTime"].isInt())
		{
			ddbSetList.sleepTime = updateVal["SleepTime"].asInt();
		}

		if (updateVal.isMember("Mode") && updateVal["Mode"].isObject())
		{
			if (updateVal["Mode"].isMember("Mode") && updateVal["Mode"]["Mode"].isInt())
			{
				ddbSetList.mode = updateVal["Mode"]["Mode"].asInt();
			}
			if (updateVal["Mode"].isMember("HostPath") && updateVal["Mode"]["HostPath"].isString())
			{
				ddbSetList.hostPath = utf8ToUtf16(updateVal["Mode"]["HostPath"].asString());
			}
			if (updateVal["Mode"].isMember("RestartHost") && updateVal["Mode"]["RestartHost"].isBool())
			{
				ddbSetList.restartHost = updateVal["Mode"]["RestartHost"].asBool();
			}
		}

		if (updateVal.isMember("Intercept") && updateVal["Intercept"].isObject())
		{
			if (updateVal["Intercept"].isMember("SeewoWhiteboard3Floating") && updateVal["Intercept"]["SeewoWhiteboard3Floating"].isBool())
			{
				ddbSetList.InterceptWindow[0] = updateVal["Intercept"]["SeewoWhiteboard3Floating"].asBool();
			}
			if (updateVal["Intercept"].isMember("SeewoWhiteboard5Floating") && updateVal["Intercept"]["SeewoWhiteboard5Floating"].isBool())
			{
				ddbSetList.InterceptWindow[1] = updateVal["Intercept"]["SeewoWhiteboard5Floating"].asBool();
			}
			if (updateVal["Intercept"].isMember("SeewoWhiteboard5CFloating") && updateVal["Intercept"]["SeewoWhiteboard5CFloating"].isBool())
			{
				ddbSetList.InterceptWindow[2] = updateVal["Intercept"]["SeewoWhiteboard5CFloating"].asBool();
			}
			if (updateVal["Intercept"].isMember("SeewoPincoFloating") && updateVal["Intercept"]["SeewoPincoFloating"].isBool())
			{
				ddbSetList.InterceptWindow[3] = ddbSetList.InterceptWindow[4] = updateVal["Intercept"]["SeewoPincoFloating"].asBool();
			}
			if (updateVal["Intercept"].isMember("SeewoPPTFloating") && updateVal["Intercept"]["SeewoPPTFloating"].isBool())
			{
				ddbSetList.InterceptWindow[5] = updateVal["Intercept"]["SeewoPPTFloating"].asBool();
			}
			if (updateVal["Intercept"].isMember("AiClassFloating") && updateVal["Intercept"]["AiClassFloating"].isBool())
			{
				ddbSetList.InterceptWindow[6] = updateVal["Intercept"]["AiClassFloating"].asBool();
			}
			if (updateVal["Intercept"].isMember("HiteAnnotationFloating") && updateVal["Intercept"]["HiteAnnotationFloating"].isBool())
			{
				ddbSetList.InterceptWindow[7] = updateVal["Intercept"]["HiteAnnotationFloating"].asBool();
			}
		}
	}

	return true;
}
bool DdbWriteSetting(HANDLE* hFile, bool close)
{
	Json::Value updateVal;

	updateVal["Edition"] = Json::Value(utf16ToUtf8(editionDate));

	updateVal["SleepTime"] = Json::Value(ddbSetList.sleepTime);

	updateVal["Mode"]["Mode"] = Json::Value(ddbSetList.mode);
	updateVal["Mode"]["HostPath"] = Json::Value(utf16ToUtf8(ddbSetList.hostPath));
	updateVal["Mode"]["RestartHost"] = Json::Value(ddbSetList.restartHost);

	updateVal["Intercept"]["SeewoWhiteboard3Floating"] = Json::Value(ddbSetList.InterceptWindow[0]);
	updateVal["Intercept"]["SeewoWhiteboard5Floating"] = Json::Value(ddbSetList.InterceptWindow[1]);
	updateVal["Intercept"]["SeewoWhiteboard5CFloating"] = Json::Value(ddbSetList.InterceptWindow[2]);
	updateVal["Intercept"]["SeewoPincoFloating"] = Json::Value(ddbSetList.InterceptWindow[3]);
	updateVal["Intercept"]["SeewoPPTFloating"] = Json::Value(ddbSetList.InterceptWindow[5]);
	updateVal["Intercept"]["AiClassFloating"] = Json::Value(ddbSetList.InterceptWindow[6]);
	updateVal["Intercept"]["HiteAnnotationFloating"] = Json::Value(ddbSetList.InterceptWindow[7]);

	updateVal["~ConfigurationChange"] = Json::Value(false);
	updateVal["~KeepOpen"] = Json::Value(!close);

	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) return false;
	if (!SetEndOfFile(hFile)) return false;

	Json::StreamWriterBuilder writerBuilder;
	string jsonContent = "\xEF\xBB\xBF" + Json::writeString(writerBuilder, updateVal);

	DWORD bytesWritten = 0;
	if (!WriteFile(hFile, jsonContent.data(), static_cast<DWORD>(jsonContent.size()), &bytesWritten, NULL) || bytesWritten != jsonContent.size()) return false;
	return true;
}