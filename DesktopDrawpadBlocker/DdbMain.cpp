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

#include "DdbConfiguration.h"
#include "DdbIntercept.h"
#include "DdbIO.h"
#include "IdtGuid.h"
#include "IdtOther.h"
#include "IdtText.h"

wstring buildTime = __DATE__ L" " __TIME__;		//����ʱ��
string editionDate = "20240507a";				//�����汾

wstring userid;									//�û�ID
string globalPath;								//�����·��
shared_ptr<spdlog::logger> DDBLogger;			//��־��¼��

bool closeSign;

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
		else if (_waccess((StringToWstring(globalPath) + L"log\\Ddb.log").c_str(), 0) == 0) filesystem::remove(StringToWstring(globalPath) + L"log\\Ddb.log", ec);

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

	// �����ļ���ʼ��
	{
		if (_waccess((StringToWstring(globalPath) + L"interaction_configuration.json").c_str(), 0) == -1 || !ConfigurationChange() || CloseSoftware())
		{
			WriteSetting(true);
			return 0;
		}

		ReadSetting();
		WriteSetting();
	}
	// ����ģʽ��ʼ��
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
	// �������ó�ʼ��
	{
		{
			// AiClass ����������
			WindowSearch[0].hasClassName = true;
			WindowSearch[0].className = L"UIWndTransparent";
			WindowSearch[0].hasWindowTitle = true;
			WindowSearch[0].windowTitle = L"TransparentWindow";
			WindowSearch[0].hasStyle = true;
			WindowSearch[0].style = -2080374784;
		}
		{
			// ϣ�ְװ� ����������
			WindowSearch[1].hasClassName = true;
			WindowSearch[1].className = L"HwndWrapper[EasiNote";
			WindowSearch[1].hasStyle = true;
			WindowSearch[1].style = 369623040;
			WindowSearch[1].hasWidthHeight = true;
			WindowSearch[1].width = 550;
			WindowSearch[1].height = 200;
		}
		{
			// ϣ��Ʒ�Σ�������������PPT�ؼ���
			WindowSearch[2].hasClassName = true;
			WindowSearch[2].className = L"Chrome_WidgetWin_1";
			WindowSearch[2].hasWindowTitle = true;
			WindowSearch[2].windowTitle = L"ϣ��Ʒ�Ρ���integration";
			WindowSearch[2].hasStyle = true;
			WindowSearch[2].style = 335675392;
		}
		{
			// ϣ��Ʒ�� ���滭��
			WindowSearch[3].hasClassName = true;
			WindowSearch[3].className = L"HwndWrapper[BoardService;;";
			WindowSearch[3].hasStyle = true;
			WindowSearch[3].style = 369623040;
		}
		{
			// ϣ��PPTС����
			WindowSearch[4].hasClassName = true;
			WindowSearch[4].className = L"HwndWrapper[PPTService.exe;;";
			WindowSearch[4].hasStyle = true;
			WindowSearch[4].style = 369623040;
		}
		WindowSearchSize = 5;
	}

	// ��ʼ����������
	for (; !closeSign; this_thread::sleep_for(chrono::milliseconds(setList.sleepTime)))
	{
		// �ȴ��ļ�δռ��
		while (IsFileUsed((globalPath + "interaction_configuration.json").c_str())) this_thread::sleep_for(chrono::milliseconds(5));
		// ��ѯ�����ļ��Ƿ��޸�
		if (ConfigurationChange())
		{
			ReadSetting();
			WriteSetting();
		}
		// ��ѯ�����Ƿ���Ҫ�ر�
		if (CloseSoftware()) break;

		// ���ش���
		bool res = DdbIntercept();

		// ��չ���ܣ�������������
		if (res && setList.mode == 0 && setList.restartHost && !isProcessRunning(setList.hostPath))
		{
			if (_waccess(setList.hostPath.c_str(), 0) == -1)
			{
				// ������������
				ShellExecute(NULL, L"runas", setList.hostPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
			}
		}
	}

	return 0;
}