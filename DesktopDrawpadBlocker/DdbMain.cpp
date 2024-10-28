/*
 * @file		DdbMain.cpp
 * @brief		DesktopDrawpadBlocker 项目中心源文件
 * @note		用于初始化 DesktopDrawpadBlocker 并调用相关模块
 *
 * @envir		VisualStudio 2022 | MSVC 143
 * @site		https://github.com/Alan-CRL/DesktopDrawpadBlocker
 *
 * @author		Alan-CRL
 * @qq			2685549821
 * @email		alan-crl@foxmail.com
 *
 * @description 项目文件中 Ddb 开头为本项目文件，Idt 开头为 智绘教Inkeys 项目引用（https://github.com/Alan-CRL/Inkeys）
*/

#include "DdbMain.h"

#include <sstream>
#include "DdbConfiguration.h"
#include "DdbIntercept.h"
#include "IdtGuid.h"
#include "IdtOther.h"
#include "IdtText.h"

#include <fstream>

wstring buildTime = __DATE__ L" " __TIME__;		//构建时间
wstring editionDate = L"20241028a";				//发布版本

wstring userid;									//用户ID
wstring globalPath;								//程序根路径

#ifndef DDB_RELEASE
void Test()
{
	MessageBoxW(NULL, L"标记处", L"标记", MB_OK | MB_SYSTEMMODAL);
}
void Testb(bool t)
{
	MessageBoxW(NULL, t ? L"true" : L"false", L"真否标记", MB_OK | MB_SYSTEMMODAL);
}
void Testi(long long t)
{
	MessageBoxW(NULL, to_wstring(t).c_str(), L"数值标记", MB_OK | MB_SYSTEMMODAL);
}
void Testw(wstring t)
{
	MessageBoxW(NULL, t.c_str(), L"字符标记", MB_OK | MB_SYSTEMMODAL);
}
void Testa(string t)
{
	MessageBoxW(NULL, utf8ToUtf16(t).c_str(), L"字符标记", MB_OK | MB_SYSTEMMODAL);
}
#endif

bool closeSign;

int WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	// 防止重复启动
	{
		if (ProcessRunningCnt(GetCurrentExePath()) > 1) return 0;
	}

	// 路径预处理
	{
		globalPath = (GetCurrentExeDirectory() + L"\\");
	}
	// 用户ID获取
	{
		userid = utf8ToUtf16(getDeviceGUID());
		if (userid.empty() || !isValidString(userid)) userid = L"IDError";
	}

	// 配置文件初始化
	{
		wstring parameters;
		wstringstream wss(GetCommandLineW());

		getline(wss, parameters, L'-');
		getline(wss, parameters, L' ');

		if (parameters == L"startup" || _waccess((globalPath + L"start_up.signal").c_str(), 0) == 0)
		{
			if (_waccess((globalPath + L"interaction_configuration.json").c_str(), 0) == -1)
			{
				DdbWriteSetting(true);
				return 0;
			}
		}
		else if (!ConfigurationChange() || CloseSoftware())
		{
			DdbWriteSetting(true);
			return 0;
		}

		DdbReadSetting();
		DdbWriteSetting();
	}
	// 程序模式初始化
	{
		if (ddbSetList.mode == 2 && !isProcessRunning(ddbSetList.hostPath))
		{
			DdbWriteSetting(true);
			return 0;
		}
		else if (ddbSetList.mode == 2) ddbSetList.hostOn = true;

		thread ddbTrackThread(DdbTrack);
		ddbTrackThread.detach();

		while (!DdbTrackReady) this_thread::sleep_for(chrono::milliseconds(10));
	}
	// 拦截配置初始化
	{
		{
			// 希沃白板3 桌面悬浮窗
			WindowSearch[0].hasWindowTitle = true;
			WindowSearch[0].windowTitle = L"Note";
			WindowSearch[0].hasClassName = true;
			WindowSearch[0].className = L"HwndWrapper[EasiNote.exe;;";
			WindowSearch[0].hasStyle = true;
			WindowSearch[0].style = 370081792;
			WindowSearch[0].hasWidthHeight = true;
			WindowSearch[0].width = GetSystemMetrics(SM_CXSCREEN);
			WindowSearch[0].height = GetSystemMetrics(SM_CYSCREEN);
		}
		{
			// 希沃白板5 桌面悬浮窗
			WindowSearch[1].hasClassName = true;
			WindowSearch[1].className = L"HwndWrapper[EasiNote;;";
			WindowSearch[1].hasStyle = true;
			WindowSearch[1].style = 369623040;
			WindowSearch[1].hasWidthHeight = true;
			WindowSearch[1].width = 550;
			WindowSearch[1].height = 200;
		}
		{
			// 希沃白板5C 桌面悬浮窗
			WindowSearch[2].hasClassName = true;
			WindowSearch[2].className = L"HwndWrapper[EasiNote5C;;";
			WindowSearch[2].hasStyle = true;
			WindowSearch[2].style = 369623040;
			WindowSearch[2].hasWidthHeight = true;
			WindowSearch[2].width = 550;
			WindowSearch[2].height = 200;
		}
		{
			// 希沃品课桌面悬浮窗（包括PPT控件）
			WindowSearch[3].hasClassName = true;
			WindowSearch[3].className = L"Chrome_WidgetWin_1";
			WindowSearch[3].hasWindowTitle = true;
			WindowSearch[3].windowTitle = L"希沃品课——integration";
			WindowSearch[3].hasStyle = true;
			WindowSearch[3].style = 335675392;
		}
		{
			// 希沃品课桌面画板（附加）
			WindowSearch[4].hasClassName = true;
			WindowSearch[4].className = L"HwndWrapper[BoardService;;";
			WindowSearch[4].hasStyle = true;
			WindowSearch[4].style = 369623040;
			WindowSearch[4].hasWidthHeight = true;
			WindowSearch[4].width = GetSystemMetrics(SM_CXSCREEN);
			WindowSearch[4].height = GetSystemMetrics(SM_CYSCREEN);
		}
		{
			// 希沃PPT小工具
			WindowSearch[5].hasClassName = true;
			WindowSearch[5].className = L"HwndWrapper[PPTService.exe;;";
			WindowSearch[5].hasStyle = true;
			WindowSearch[5].style = 369623040;
		}
		{
			// AiClass 桌面悬浮窗
			WindowSearch[6].hasClassName = true;
			WindowSearch[6].className = L"UIWndTransparent";
			WindowSearch[6].hasWindowTitle = true;
			WindowSearch[6].windowTitle = L"TransparentWindow";
			WindowSearch[6].hasStyle = true;
			WindowSearch[6].style = -2080374784;
		}
		{
			// 鸿合屏幕书写
			WindowSearch[7].hasWindowTitle = true;
			WindowSearch[7].windowTitle = L"HiteAnnotation";
			WindowSearch[7].hasClassName = true;
			WindowSearch[7].className = L"Qt5QWindowToolSaveBits";
			WindowSearch[7].hasStyle = true;
			WindowSearch[7].style = -1778384896;
		}
		WindowSearchSize = 8;
	}

	// 开始拦截悬浮窗
	for (; !closeSign; this_thread::sleep_for(chrono::milliseconds(ddbSetList.sleepTime)))
	{
		// 查询程序是否需要关闭
		if (CloseSoftware()) break;

		// 查询配置文件是否修改
		if (ConfigurationChange())
		{
			DdbReadSetting();
			DdbWriteSetting();
		}

		// 拦截窗口
		bool res = DdbIntercept();

		// 扩展功能：重启宿主程序
		if (res && ddbSetList.mode == 0 && ddbSetList.restartHost && !isProcessRunning(ddbSetList.hostPath))
		{
			if (ddbSetList.hostPath != L"CommissioningTest" && _waccess(ddbSetList.hostPath.c_str(), 0) == 0)
			{
				// 重启宿主程序
				ShellExecuteW(NULL, NULL, ddbSetList.hostPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
			}
		}
	}

	DdbWriteSetting(true);
	return 0;
}