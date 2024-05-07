#include "IdtOther.h"

#include <tlhelp32.h>
#include "DdbConfiguration.h"

wstring GetCurrentExeDirectory()
{
	wchar_t buffer[MAX_PATH];
	DWORD length = GetModuleFileNameW(NULL, buffer, sizeof(buffer) / sizeof(wchar_t));
	if (length == 0 || length == sizeof(buffer) / sizeof(wchar_t)) return L"";

	filesystem::path fullPath(buffer);
	return fullPath.parent_path().wstring();
}
wstring GetCurrentExePath()
{
	wchar_t buffer[MAX_PATH];
	DWORD length = GetModuleFileNameW(NULL, buffer, sizeof(buffer) / sizeof(wchar_t));
	if (length == 0 || length == sizeof(buffer) / sizeof(wchar_t)) return L"";

	return (wstring)buffer;
}
wstring GetCurrentExeName()
{
	wchar_t buffer[MAX_PATH];
	DWORD length = GetModuleFileNameW(NULL, buffer, sizeof(buffer) / sizeof(wchar_t));
	if (length == 0 || length == sizeof(buffer) / sizeof(wchar_t)) return L"";

	filesystem::path fullPath(buffer);
	return fullPath.filename().wstring();
}

bool isValidString(const wstring& str)
{
	for (wchar_t ch : str)
	{
		if (!iswprint(ch) && !iswspace(ch))
			return false;
	}
	return true;
}

//程序进程状态获取
bool isProcessRunning(const std::wstring& processPath)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry)) {
		do {
			// 打开进程句柄
			HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, entry.th32ProcessID);
			if (process == NULL) {
				continue;
			}

			// 获取进程完整路径
			wchar_t path[MAX_PATH];
			DWORD size = MAX_PATH;
			if (QueryFullProcessImageName(process, 0, path, &size)) {
				if (processPath == path) {
					CloseHandle(process);
					CloseHandle(snapshot);
					return true;
				}
			}

			CloseHandle(process);
		} while (Process32Next(snapshot, &entry));
	}

	CloseHandle(snapshot);
	return false;
}
void DdbTrack()
{
	for (;;)
	{
		if (isProcessRunning(setList.hostPath)) setList.hostOn = true;
		else if (setList.hostOn) closeSign = true;

		this_thread::sleep_for(chrono::milliseconds(500));
	}
}