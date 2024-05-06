#pragma once

#include "DdbMain.h"

struct SetListStruct
{
	SetListStruct()
	{
		sleepTime = 5000;

		mode = 0;
		hostOn = false;
		hostPath = L"";

		memset(InterceptWindow, true, sizeof(InterceptWindow));
	}

	int sleepTime;

	int mode; // 0 独立模式 1 随主程序开启关闭 2 随主程序关闭
	wstring hostPath;
	int hostOn;

	bool InterceptWindow[10];
	/* InterceptWindow 列表：
	 *
	 * 0 AiClass 桌面悬浮窗
	 * 1 希沃白板 桌面悬浮窗
	 * 2 希沃品课（桌面悬浮窗和PPT控件）
	 * 3 希沃品课 桌面画板
	 * 4 希沃PPT小工具
	 *
	 */
};
extern SetListStruct setList;

bool ConfigurationChange();
bool CloseSoftware();

bool ReadSetting();
bool WriteSetting(bool close = false);