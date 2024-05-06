/*
 * @file		DdbMain.cpp
 * @brief		DesktopDrawpadBlocker ��Ŀ����Դ�ļ�
 * @note		���ڳ�ʼ�� DesktopDrawpadBlocker ���������ģ��
 *
 * @envir		VisualStudio 2022 | MSVC 143
 * @site		https://github.com/Alan-CRL/DesktopDrawpadBlocker
 *
 * @author		Alan-CRL
 * @qq			2685549821
 * @email		alan-crl@foxmail.com
 *
 * @description ��Ŀ�ļ��� Ddb ��ͷΪ����Ŀ�ļ���Idt ��ͷΪ �ǻ��Inkeys ��Ŀ���ã�https://github.com/Alan-CRL/Intelligent-Drawing-Teaching��
*/

#include "DdbMain.h"

#include "IdtText.h"
#include "IdtOther.h"
#include "IdtGuid.h"

wstring buildTime = __DATE__ L" " __TIME__;		//����ʱ��
string editionDate = "20240505a";				//�����汾

wstring userid;									//�û�ID
string globalPath;								//�����·��
shared_ptr<spdlog::logger> DDBLogger;

int WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	// ·��Ԥ����
	{
		globalPath = WstringToString(GetCurrentExeDirectory() + L"\\");
	}
	// �û�ID��ȡ
	{
		userid = StringToWstring(getDeviceGUID());
		if (userid.empty() || !isValidString(userid)) userid = L"IDError";
	}
	// ��־�����ʼ��
	{
		error_code ec;
		if (_waccess((StringToWstring(globalPath) + L"log").c_str(), 0) == -1) filesystem::create_directory(StringToWstring(globalPath) + L"log", ec);

		auto DDBLoggerFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(globalPath + "log\\Ddb.log");

		spdlog::init_thread_pool(8192, 64);
		DDBLogger = std::make_shared<spdlog::async_logger>("DDBLogger", DDBLoggerFileSink, spdlog::thread_pool(), spdlog::async_overflow_policy::block);

		DDBLogger->set_level(spdlog::level::info);
		DDBLogger->set_pattern("[%l][%H:%M:%S.%e]%v");

		DDBLogger->flush_on(spdlog::level::info);
		DDBLogger->info("[���߳�][DdbMain] ��־��ʼ��¼");
		DDBLogger->info("[���߳�][DdbMain] DDB�汾 " + editionDate);
		DDBLogger->info("[���߳�][DdbMain] �û�ID " + WstringToString(userid));

		//logger->info("");
		//logger->warn("");
		//logger->error("");
		//logger->critical("");
	}

	return 0;
}