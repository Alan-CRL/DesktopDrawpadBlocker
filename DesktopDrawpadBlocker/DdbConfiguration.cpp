#include "DdbConfiguration.h"

#include "IdtText.h"
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
			if (updateVal["Intercept"].isMember("SeewoPincoSideBarFloating") && updateVal["Intercept"]["SeewoPincoSideBarFloating"].isBool())
			{
				ddbSetList.InterceptWindow[3] = updateVal["Intercept"]["SeewoPincoSideBarFloating"].asBool();
			}
			if (updateVal["Intercept"].isMember("SeewoPincoDrawingFloating") && updateVal["Intercept"]["SeewoPincoDrawingFloating"].isBool())
			{
				bool temp = updateVal["Intercept"]["SeewoPincoDrawingFloating"].asBool();
				ddbSetList.InterceptWindow[4] = temp;
				ddbSetList.InterceptWindow[5] = temp;
			}
			if (updateVal["Intercept"].isMember("SeewoPPTFloating") && updateVal["Intercept"]["SeewoPPTFloating"].isBool())
			{
				ddbSetList.InterceptWindow[6] = updateVal["Intercept"]["SeewoPPTFloating"].asBool();
			}
			if (updateVal["Intercept"].isMember("AiClassFloating") && updateVal["Intercept"]["AiClassFloating"].isBool())
			{
				ddbSetList.InterceptWindow[7] = updateVal["Intercept"]["AiClassFloating"].asBool();
			}
			if (updateVal["Intercept"].isMember("HiteAnnotationFloating") && updateVal["Intercept"]["HiteAnnotationFloating"].isBool())
			{
				ddbSetList.InterceptWindow[8] = updateVal["Intercept"]["HiteAnnotationFloating"].asBool();
			}
			if (updateVal["Intercept"].isMember("ChangYanFloating") && updateVal["Intercept"]["ChangYanFloating"].isBool())
			{
				bool temp = updateVal["Intercept"]["ChangYanFloating"].asBool();
				ddbSetList.InterceptWindow[9] = temp;
				ddbSetList.InterceptWindow[10] = temp;
				ddbSetList.InterceptWindow[11] = temp;
				ddbSetList.InterceptWindow[12] = temp;
				ddbSetList.InterceptWindow[13] = temp;
				ddbSetList.InterceptWindow[14] = temp;
				ddbSetList.InterceptWindow[15] = temp;
				ddbSetList.InterceptWindow[16] = temp;
				ddbSetList.InterceptWindow[17] = temp;
				ddbSetList.InterceptWindow[18] = temp;
				ddbSetList.InterceptWindow[19] = temp;
				ddbSetList.InterceptWindow[20] = temp;
				ddbSetList.InterceptWindow[21] = temp;
			}
			if (updateVal["Intercept"].isMember("IntelligentClassFloating") && updateVal["Intercept"]["IntelligentClassFloating"].isBool())
			{
				bool temp = updateVal["Intercept"]["IntelligentClassFloating"].asBool();
				ddbSetList.InterceptWindow[22] = temp;
				ddbSetList.InterceptWindow[23] = temp;
				ddbSetList.InterceptWindow[24] = temp;
			}
			if (updateVal["Intercept"].isMember("SeewoDesktopAnnotationFloating") && updateVal["Intercept"]["SeewoDesktopAnnotationFloating"].isBool())
			{
				ddbSetList.InterceptWindow[25] = updateVal["Intercept"]["SeewoDesktopAnnotationFloating"].asBool();
			}
			if (updateVal["Intercept"].isMember("SeewoDesktopSideBarFloating") && updateVal["Intercept"]["SeewoDesktopSideBarFloating"].isBool())
			{
				ddbSetList.InterceptWindow[26] = updateVal["Intercept"]["SeewoDesktopSideBarFloating"].asBool();
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

		updateVal["Intercept"]["SeewoWhiteboard3Floating"] = Json::Value(ddbSetList.InterceptWindow[0]);
		updateVal["Intercept"]["SeewoWhiteboard5Floating"] = Json::Value(ddbSetList.InterceptWindow[1]);
		updateVal["Intercept"]["SeewoWhiteboard5CFloating"] = Json::Value(ddbSetList.InterceptWindow[2]);
		updateVal["Intercept"]["SeewoPincoSideBarFloating"] = Json::Value(ddbSetList.InterceptWindow[3]);
		updateVal["Intercept"]["SeewoPincoDrawingFloating"] = Json::Value(ddbSetList.InterceptWindow[4]);
		updateVal["Intercept"]["SeewoPPTFloating"] = Json::Value(ddbSetList.InterceptWindow[6]);
		updateVal["Intercept"]["AiClassFloating"] = Json::Value(ddbSetList.InterceptWindow[7]);
		updateVal["Intercept"]["HiteAnnotationFloating"] = Json::Value(ddbSetList.InterceptWindow[8]);
		updateVal["Intercept"]["ChangYanFloating"] = Json::Value(ddbSetList.InterceptWindow[9]);
		updateVal["Intercept"]["IntelligentClassFloating"] = Json::Value(ddbSetList.InterceptWindow[22]);
		updateVal["Intercept"]["SeewoDesktopAnnotationFloating"] = Json::Value(ddbSetList.InterceptWindow[25]);
		updateVal["Intercept"]["SeewoDesktopSideBarFloating"] = Json::Value(ddbSetList.InterceptWindow[26]);

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