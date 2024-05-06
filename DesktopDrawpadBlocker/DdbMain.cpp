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

#include "IdtText.h"
#include "IdtOther.h"
#include "IdtGuid.h"

wstring buildTime = __DATE__ L" " __TIME__;		//构建时间
string editionDate = "20240505a";				//发布版本

wstring userid;									//用户ID
string globalPath;								//程序根路径
shared_ptr<spdlog::logger> DDBLogger;

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

	return 0;
}