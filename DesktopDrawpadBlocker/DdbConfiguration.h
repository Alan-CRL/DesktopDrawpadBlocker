#pragma once

#include "DdbMain.h"

struct DdbSetListStruct
{
	// Ddb �����ļ��汾 20240509a

	DdbSetListStruct()
	{
		sleepTime = 5000;

		mode = 0;
		hostPath = L"";
		hostOn = false, restartHost = true;

		memset(InterceptWindow, true, sizeof(InterceptWindow));
	}

	int sleepTime;

	int mode; // 0 ����ģʽ 1 ����������Ϳ����͹ر� 2 ����������ر�
	wstring hostPath;
	bool hostOn, restartHost; // restartHost�������޶���ģʽ�����������򱻹رպ����ص���������Ĵ��ں�������������

	bool InterceptWindow[10];
};
extern DdbSetListStruct ddbSetList;

bool ConfigurationChange();
bool CloseSoftware();

bool DdbReadSetting();
bool DdbWriteSetting(bool close = false);