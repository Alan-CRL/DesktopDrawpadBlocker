#pragma once

#include "DdbMain.h"

struct DdbSetListStruct
{
	// Ddb 配置文件版本 20241028a

	DdbSetListStruct()
	{
		sleepTime = 5000;

		mode = 0;
		hostPath = L"";
		hostOn = false, restartHost = false;

		memset(InterceptWindow, true, sizeof(InterceptWindow));
	}

	int sleepTime;

	int mode; // 0 独立模式 1 随宿主程序和开启和关闭 2 随宿主程序关闭
	wstring hostPath;
	bool hostOn, restartHost; // restartHost：（仅限独立模式）当宿主程序被关闭后，拦截到其他软件的窗口后，重启宿主程序

	bool InterceptWindow[40];
};
extern DdbSetListStruct ddbSetList;

extern bool usingBom;

bool OccupyFileForRead(HANDLE* hFile, const wstring& filePath);
bool OccupyFileForWrite(HANDLE* hFile, const wstring& filePath);
bool UnOccupyFile(HANDLE* hFile);

bool ConfigurationChange();
bool CloseSoftware();

bool DdbReadSetting();
bool DdbWriteSetting(bool close = false);