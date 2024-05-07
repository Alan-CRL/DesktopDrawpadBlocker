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
 * @description 项目文件中 Ddb 开头为本项目文件，Idt 开头为 智绘教Inkeys 项目引用（https://github.com/Alan-CRL/Intelligent-Drawing-Teaching）
*/

#include "DdbMain.h"

#include "DdbConfiguration.h"
#include "DdbIntercept.h"
#include "DdbIO.h"
#include "IdtGuid.h"
#include "IdtOther.h"
#include "IdtText.h"

wstring buildTime = __DATE__ L" " __TIME__;		//构建时间
string editionDate = "20240507a";				//发布版本

wstring userid;									//用户ID
string globalPath;								//程序根路径
shared_ptr<spdlog::logger> DDBLogger;			//日志记录器

bool closeSign;

int WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	// 路径预处理
	{
		globalPath = WstringToString(GetCurrentExeDirectory() + L"\\");
	}
	// 用户ID获取
	{
		userid = StringToWstring(getDeviceGUID());
		if (userid.empty() || !isValidString(userid)) userid = L"IDError";
	}
	// 日志服务初始化
	{
		error_code ec;
		if (_waccess((StringToWstring(globalPath) + L"log").c_str(), 0) == -1) filesystem::create_directory(StringToWstring(globalPath) + L"log", ec);
		else if (_waccess((StringToWstring(globalPath) + L"log\\Ddb.log").c_str(), 0) == 0) filesystem::remove(StringToWstring(globalPath) + L"log\\Ddb.log", ec);

		auto DDBLoggerFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(globalPath + "log\\Ddb.log");

		spdlog::init_thread_pool(8192, 64);
		DDBLogger = std::make_shared<spdlog::async_logger>("DDBLogger", DDBLoggerFileSink, spdlog::thread_pool(), spdlog::async_overflow_policy::block);

		DDBLogger->set_level(spdlog::level::info);
		DDBLogger->set_pattern("[%l][%H:%M:%S.%e]%v");

		DDBLogger->flush_on(spdlog::level::info);
		DDBLogger->info("[主线程][DdbMain] 日志开始记录");
		DDBLogger->info("[主线程][DdbMain] DDB版本 " + editionDate);
		DDBLogger->info("[主线程][DdbMain] 用户ID " + WstringToString(userid));

		//logger->info("");
		//logger->warn("");
		//logger->error("");
		//logger->critical("");
	}

	// 配置文件初始化
	{
		if (_waccess((StringToWstring(globalPath) + L"interaction_configuration.json").c_str(), 0) == -1 || !ConfigurationChange() || CloseSoftware())
		{
			WriteSetting(true);
			return 0;
		}

		ReadSetting();
		WriteSetting();
	}
	// 程序模式初始化
	{
		if (setList.mode == 2 && !isProcessRunning(setList.hostPath)) return 0;
		else if (setList.mode == 2) setList.hostOn = true;

		if (setList.mode == 1 || setList.mode == 2)
		{
			if (_waccess(setList.hostPath.c_str(), 0) == -1) return 0;

			thread ddbTrackThread(DdbTrack);
			ddbTrackThread.detach();
		}
		else if (setList.mode == 0 && setList.restartHost)
		{
			if (_waccess(setList.hostPath.c_str(), 0) == -1) return 0;
		}
	}
	// 拦截配置初始化
	{
		{
			// AiClass 桌面悬浮窗
			WindowSearch[0].hasClassName = true;
			WindowSearch[0].className = L"UIWndTransparent";
			WindowSearch[0].hasWindowTitle = true;
			WindowSearch[0].windowTitle = L"TransparentWindow";
			WindowSearch[0].hasStyle = true;
			WindowSearch[0].style = -2080374784;
		}
		{
			// 希沃白板 桌面悬浮窗
			WindowSearch[1].hasClassName = true;
			WindowSearch[1].className = L"HwndWrapper[EasiNote";
			WindowSearch[1].hasStyle = true;
			WindowSearch[1].style = 369623040;
			WindowSearch[1].hasWidthHeight = true;
			WindowSearch[1].width = 550;
			WindowSearch[1].height = 200;
		}
		{
			// 希沃品课（桌面悬浮窗和PPT控件）
			WindowSearch[2].hasClassName = true;
			WindowSearch[2].className = L"Chrome_WidgetWin_1";
			WindowSearch[2].hasWindowTitle = true;
			WindowSearch[2].windowTitle = L"希沃品课——integration";
			WindowSearch[2].hasStyle = true;
			WindowSearch[2].style = 335675392;
		}
		{
			// 希沃品课 桌面画板
			WindowSearch[3].hasClassName = true;
			WindowSearch[3].className = L"HwndWrapper[BoardService;;";
			WindowSearch[3].hasStyle = true;
			WindowSearch[3].style = 369623040;
		}
		{
			// 希沃PPT小工具
			WindowSearch[4].hasClassName = true;
			WindowSearch[4].className = L"HwndWrapper[PPTService.exe;;";
			WindowSearch[4].hasStyle = true;
			WindowSearch[4].style = 369623040;
		}
		WindowSearchSize = 5;
	}

	// 开始拦截悬浮窗
	for (; !closeSign; this_thread::sleep_for(chrono::milliseconds(setList.sleepTime)))
	{
		// 等待文件未占用
		while (IsFileUsed((globalPath + "interaction_configuration.json").c_str())) this_thread::sleep_for(chrono::milliseconds(5));
		// 查询配置文件是否修改
		if (ConfigurationChange())
		{
			ReadSetting();
			WriteSetting();
		}
		// 查询程序是否需要关闭
		if (CloseSoftware()) break;

		// 拦截窗口
		bool res = DdbIntercept();

		// 扩展功能：重启宿主程序
		if (res && setList.mode == 0 && setList.restartHost && !isProcessRunning(setList.hostPath))
		{
			if (_waccess(setList.hostPath.c_str(), 0) == -1)
			{
				// 重启宿主程序
				ShellExecute(NULL, L"runas", setList.hostPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
			}
		}
	}

	return 0;
}