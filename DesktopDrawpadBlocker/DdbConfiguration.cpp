#include "DdbConfiguration.h"

#include "IdtText.h"
#include "DdbIntercept.h"
#include "JsonCpp/json.h"

#include <fstream>

DdbSetListStruct ddbSetList;
bool usingBom = false;

// 占用文件
bool OccupyFileForRead(HANDLE* hFile, const wstring& filePath)
{
	if (!filesystem::exists(filePath)) return false;

	for (int time = 1; time <= 5; time++)
	{
		*hFile = CreateFileW(
			filePath.c_str(),
			GENERIC_READ,
			0,              // 不共享，独占访问
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);

		if (*hFile != INVALID_HANDLE_VALUE) break;
		else if (time >= 3) return false;

		this_thread::sleep_for(chrono::milliseconds(100));
	}

	if (*hFile != INVALID_HANDLE_VALUE) return true;
	return false;
}
bool OccupyFileForWrite(HANDLE* hFile, const wstring& filePath)
{
	filesystem::path directoryPath = filesystem::path(filePath).parent_path();
	if (!filesystem::exists(directoryPath)) {
		error_code ec;
		filesystem::create_directories(directoryPath, ec);
	}

	for (int time = 1; time <= 5; time++)
	{
		*hFile = CreateFileW(
			filePath.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,              // 不共享，独占访问
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);

		if (*hFile != INVALID_HANDLE_VALUE) break;
		else if (time >= 3) return false;

		this_thread::sleep_for(chrono::milliseconds(100));
	}

	if (*hFile != INVALID_HANDLE_VALUE) return true;
	return false;
}
// 释放文件
bool UnOccupyFile(HANDLE* hFile)
{
	if (*hFile != NULL)
	{
		CloseHandle(*hFile);
		return true;
	}
	return false;
}

bool ConfigurationChange()
{
	bool ret = false;

	HANDLE fileHandle = NULL;
	if (!OccupyFileForRead(&fileHandle, globalPath + L"interaction_configuration.json"))
	{
		UnOccupyFile(&fileHandle);
		return ret;
	}

	LARGE_INTEGER fileSize;
	if (!GetFileSizeEx(fileHandle, &fileSize))
	{
		UnOccupyFile(&fileHandle);
		return ret;
	}

	DWORD dwSize = static_cast<DWORD>(fileSize.QuadPart);
	string jsonContent = string(dwSize, '\0');

	DWORD bytesRead = 0;
	if (SetFilePointer(fileHandle, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		UnOccupyFile(&fileHandle);
		return ret;
	}
	if (!ReadFile(fileHandle, &jsonContent[0], dwSize, &bytesRead, NULL) || bytesRead != dwSize)
	{
		UnOccupyFile(&fileHandle);
		return ret;
	}

	if (jsonContent.compare(0, 3, "\xEF\xBB\xBF") == 0) jsonContent = jsonContent.substr(3), usingBom = true;
	UnOccupyFile(&fileHandle);

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
bool CloseSoftware()
{
	bool ret = true;

	HANDLE fileHandle = NULL;
	if (!OccupyFileForRead(&fileHandle, globalPath + L"interaction_configuration.json"))
	{
		UnOccupyFile(&fileHandle);
		return ret;
	}

	LARGE_INTEGER fileSize;
	if (!GetFileSizeEx(fileHandle, &fileSize))
	{
		UnOccupyFile(&fileHandle);
		return ret;
	}

	DWORD dwSize = static_cast<DWORD>(fileSize.QuadPart);
	string jsonContent = string(dwSize, '\0');

	DWORD bytesRead = 0;
	if (SetFilePointer(fileHandle, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		UnOccupyFile(&fileHandle);
		return ret;
	}
	if (!ReadFile(fileHandle, &jsonContent[0], dwSize, &bytesRead, NULL) || bytesRead != dwSize)
	{
		UnOccupyFile(&fileHandle);
		return ret;
	}

	if (jsonContent.compare(0, 3, "\xEF\xBB\xBF") == 0) jsonContent = jsonContent.substr(3), usingBom = true;
	UnOccupyFile(&fileHandle);

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

bool DdbReadSetting()
{
	HANDLE fileHandle = NULL;
	if (!OccupyFileForRead(&fileHandle, globalPath + L"interaction_configuration.json"))
	{
		UnOccupyFile(&fileHandle);
		return false;
	}

	LARGE_INTEGER fileSize;
	if (!GetFileSizeEx(fileHandle, &fileSize))
	{
		UnOccupyFile(&fileHandle);
		return false;
	}

	DWORD dwSize = static_cast<DWORD>(fileSize.QuadPart);
	string jsonContent = string(dwSize, '\0');

	DWORD bytesRead = 0;
	if (SetFilePointer(fileHandle, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		UnOccupyFile(&fileHandle);
		return false;
	}
	if (!ReadFile(fileHandle, &jsonContent[0], dwSize, &bytesRead, NULL) || bytesRead != dwSize)
	{
		UnOccupyFile(&fileHandle);
		return false;
	}

	if (jsonContent.compare(0, 3, "\xEF\xBB\xBF") == 0) jsonContent = jsonContent.substr(3), usingBom = true;
	UnOccupyFile(&fileHandle);

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
				windowUnionList[InterceptObjectEnum::SeewoWhiteboard3Floating].enable = updateVal["Intercept"]["SeewoWhiteboard3Floating"].asBool();
			}
			if (updateVal["Intercept"].isMember("SeewoWhiteboard5Floating") && updateVal["Intercept"]["SeewoWhiteboard5Floating"].isBool())
			{
				windowUnionList[InterceptObjectEnum::SeewoWhiteboard5Floating].enable = updateVal["Intercept"]["SeewoWhiteboard5Floating"].asBool();
			}
			if (updateVal["Intercept"].isMember("SeewoWhiteboard5CFloating") && updateVal["Intercept"]["SeewoWhiteboard5CFloating"].isBool())
			{
				windowUnionList[InterceptObjectEnum::SeewoWhiteboard5CFloating].enable = updateVal["Intercept"]["SeewoWhiteboard5CFloating"].asBool();
			}
			if (updateVal["Intercept"].isMember("SeewoPincoSideBarFloating") && updateVal["Intercept"]["SeewoPincoSideBarFloating"].isBool())
			{
				windowUnionList[InterceptObjectEnum::SeewoPincoSideBarFloating].enable = updateVal["Intercept"]["SeewoPincoSideBarFloating"].asBool();
			}
			if (updateVal["Intercept"].isMember("SeewoPincoDrawingFloating") && updateVal["Intercept"]["SeewoPincoDrawingFloating"].isBool())
			{
				windowUnionList[InterceptObjectEnum::SeewoPincoDrawingFloating].enable = updateVal["Intercept"]["SeewoPincoDrawingFloating"].asBool();
			}
			if (updateVal["Intercept"].isMember("SeewoPPTFloating") && updateVal["Intercept"]["SeewoPPTFloating"].isBool())
			{
				windowUnionList[InterceptObjectEnum::SeewoPPTFloating].enable = updateVal["Intercept"]["SeewoPPTFloating"].asBool();
			}
			if (updateVal["Intercept"].isMember("SeewoIwbAssistantFloating") && updateVal["Intercept"]["SeewoIwbAssistantFloating"].isBool())
			{
				windowUnionList[InterceptObjectEnum::SeewoIwbAssistantFloating].enable = updateVal["Intercept"]["SeewoIwbAssistantFloating"].asBool();
			}
			if (updateVal["Intercept"].isMember("YiouBoardFloating") && updateVal["Intercept"]["YiouBoardFloating"].isBool())
			{
				windowUnionList[InterceptObjectEnum::YiouBoardFloating].enable = updateVal["Intercept"]["YiouBoardFloating"].asBool();
			}
			if (updateVal["Intercept"].isMember("AiClassFloating") && updateVal["Intercept"]["AiClassFloating"].isBool())
			{
				windowUnionList[InterceptObjectEnum::AiClassFloating].enable = updateVal["Intercept"]["AiClassFloating"].asBool();
			}
			if (updateVal["Intercept"].isMember("ClassInXFloating") && updateVal["Intercept"]["ClassInXFloating"].isBool())
			{
				windowUnionList[InterceptObjectEnum::ClassInXFloating].enable = updateVal["Intercept"]["ClassInXFloating"].asBool();
			}
			if (updateVal["Intercept"].isMember("IntelligentClassFloating") && updateVal["Intercept"]["IntelligentClassFloating"].isBool())
			{
				windowUnionList[InterceptObjectEnum::IntelligentClassFloating].enable = updateVal["Intercept"]["IntelligentClassFloating"].asBool();
			}
			if (updateVal["Intercept"].isMember("ChangYanFloating") && updateVal["Intercept"]["ChangYanFloating"].isBool())
			{
				windowUnionList[InterceptObjectEnum::ChangYanFloating].enable = updateVal["Intercept"]["ChangYanFloating"].asBool();
			}
			if (updateVal["Intercept"].isMember("ChangYan5Floating") && updateVal["Intercept"]["ChangYan5Floating"].isBool())
			{
				windowUnionList[InterceptObjectEnum::ChangYan5Floating].enable = updateVal["Intercept"]["ChangYan5Floating"].asBool();
			}
			if (updateVal["Intercept"].isMember("Iclass30SidebarFloating") && updateVal["Intercept"]["Iclass30SidebarFloating"].isBool())
			{
				windowUnionList[InterceptObjectEnum::Iclass30SidebarFloating].enable = updateVal["Intercept"]["Iclass30SidebarFloating"].asBool();
			}
			if (updateVal["Intercept"].isMember("Iclass30Floating") && updateVal["Intercept"]["Iclass30Floating"].isBool())
			{
				windowUnionList[InterceptObjectEnum::Iclass30Floating].enable = updateVal["Intercept"]["Iclass30Floating"].asBool();
			}
			if (updateVal["Intercept"].isMember("SeewoDesktopSideBarFloating") && updateVal["Intercept"]["SeewoDesktopSideBarFloating"].isBool())
			{
				windowUnionList[InterceptObjectEnum::SeewoDesktopSideBarFloating].enable = updateVal["Intercept"]["SeewoDesktopSideBarFloating"].asBool();
			}
			if (updateVal["Intercept"].isMember("SeewoDesktopDrawingFloating") && updateVal["Intercept"]["SeewoDesktopDrawingFloating"].isBool())
			{
				windowUnionList[InterceptObjectEnum::SeewoDesktopDrawingFloating].enable = updateVal["Intercept"]["SeewoDesktopDrawingFloating"].asBool();
			}
		}
	}
	else return false;

	return true;
}
bool DdbWriteSetting(bool close)
{
	Json::Value updateVal;
	{
		updateVal["Edition"] = Json::Value(utf16ToUtf8(editionDate));

		updateVal["SleepTime"] = Json::Value(ddbSetList.sleepTime);

		updateVal["Mode"]["Mode"] = Json::Value(ddbSetList.mode);
		updateVal["Mode"]["HostPath"] = Json::Value(utf16ToUtf8(ddbSetList.hostPath));
		updateVal["Mode"]["RestartHost"] = Json::Value(ddbSetList.restartHost);

		updateVal["Intercept"]["SeewoWhiteboard3Floating"] = Json::Value(windowUnionList[InterceptObjectEnum::SeewoWhiteboard3Floating].enable);
		updateVal["Intercept"]["SeewoWhiteboard5Floating"] = Json::Value(windowUnionList[InterceptObjectEnum::SeewoWhiteboard5Floating].enable);
		updateVal["Intercept"]["SeewoWhiteboard5CFloating"] = Json::Value(windowUnionList[InterceptObjectEnum::SeewoWhiteboard5CFloating].enable);
		updateVal["Intercept"]["SeewoPincoSideBarFloating"] = Json::Value(windowUnionList[InterceptObjectEnum::SeewoPincoSideBarFloating].enable);
		updateVal["Intercept"]["SeewoPincoDrawingFloating"] = Json::Value(windowUnionList[InterceptObjectEnum::SeewoPincoDrawingFloating].enable);
		updateVal["Intercept"]["SeewoPPTFloating"] = Json::Value(windowUnionList[InterceptObjectEnum::SeewoPPTFloating].enable);
		updateVal["Intercept"]["SeewoIwbAssistantFloating"] = Json::Value(windowUnionList[InterceptObjectEnum::SeewoIwbAssistantFloating].enable);
		updateVal["Intercept"]["YiouBoardFloating"] = Json::Value(windowUnionList[InterceptObjectEnum::YiouBoardFloating].enable);
		updateVal["Intercept"]["AiClassFloating"] = Json::Value(windowUnionList[InterceptObjectEnum::AiClassFloating].enable);
		updateVal["Intercept"]["ClassInXFloating"] = Json::Value(windowUnionList[InterceptObjectEnum::ClassInXFloating].enable);
		updateVal["Intercept"]["IntelligentClassFloating"] = Json::Value(windowUnionList[InterceptObjectEnum::IntelligentClassFloating].enable);
		updateVal["Intercept"]["ChangYanFloating"] = Json::Value(windowUnionList[InterceptObjectEnum::ChangYanFloating].enable);
		updateVal["Intercept"]["ChangYan5Floating"] = Json::Value(windowUnionList[InterceptObjectEnum::ChangYan5Floating].enable);
		updateVal["Intercept"]["Iclass30SidebarFloating"] = Json::Value(windowUnionList[InterceptObjectEnum::Iclass30SidebarFloating].enable);
		updateVal["Intercept"]["Iclass30Floating"] = Json::Value(windowUnionList[InterceptObjectEnum::Iclass30Floating].enable);
		updateVal["Intercept"]["SeewoDesktopSideBarFloating"] = Json::Value(windowUnionList[InterceptObjectEnum::SeewoDesktopSideBarFloating].enable);
		updateVal["Intercept"]["SeewoDesktopDrawingFloating"] = Json::Value(windowUnionList[InterceptObjectEnum::SeewoDesktopDrawingFloating].enable);

		updateVal["~ConfigurationChange"] = Json::Value(false);
		updateVal["~KeepOpen"] = Json::Value(!close);
	}

	HANDLE fileHandle = NULL;
	if (!OccupyFileForWrite(&fileHandle, globalPath + L"interaction_configuration.json"))
	{
		UnOccupyFile(&fileHandle);
		return false;
	}
	if (SetFilePointer(fileHandle, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		UnOccupyFile(&fileHandle);
		return false;
	}
	if (!SetEndOfFile(fileHandle))
	{
		UnOccupyFile(&fileHandle);
		return false;
	}

	Json::StreamWriterBuilder writerBuilder;
	string jsonContent = Json::writeString(writerBuilder, updateVal);
	if (usingBom) jsonContent = "\xEF\xBB\xBF" + jsonContent;

	DWORD bytesWritten = 0;
	if (!WriteFile(fileHandle, jsonContent.data(), static_cast<DWORD>(jsonContent.size()), &bytesWritten, NULL) || bytesWritten != jsonContent.size())
	{
		UnOccupyFile(&fileHandle);
		return false;
	}

	UnOccupyFile(&fileHandle);
	return true;
}