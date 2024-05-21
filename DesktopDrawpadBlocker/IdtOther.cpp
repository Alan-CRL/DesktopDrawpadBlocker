#include "IdtOther.h"

#include <tlhelp32.h>
#include <psapi.h>
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

// �������״̬��ȡ
bool isProcessRunning(const std::wstring& processPath)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry)) {
		do {
			// �򿪽��̾��
			HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, entry.th32ProcessID);
			if (process == NULL) {
				continue;
			}

			// ��ȡ��������·��
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
// ���̳���·����ѯ
int ProcessRunningCnt(const std::wstring& processPath)
{
	int ret = 0;

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
	{
		while (Process32Next(snapshot, &entry))
		{
			// ��ȡ���̵�����·��
			wchar_t processFullPath[MAX_PATH] = L"";

			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, entry.th32ProcessID);
			if (hProcess)
			{
				HMODULE hMod;
				DWORD cbNeeded;
				if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
				{
					GetModuleFileNameExW(hProcess, hMod, processFullPath, MAX_PATH);
				}
				CloseHandle(hProcess);
			}

			// �Ƚ�·���Ƿ���ͬ
			if (wcslen(processFullPath) > 0 && wcscmp(processFullPath, processPath.c_str()) == 0) ret++;
		}
	}

	CloseHandle(snapshot);
	return ret;
}

bool DdbTrackReady;
void DdbTrack()
{
	// ������������Ƿ����
	if (ddbSetList.mode == 1 || (ddbSetList.mode == 0 && ddbSetList.restartHost))
	{
		if (_waccess(ddbSetList.hostPath.c_str(), 0) == -1 && ddbSetList.hostPath != L"CommissioningTest")
			closeSign = true;
	}
	// ������������Ƿ�������
	if (ddbSetList.mode != 0)
	{
		if (isProcessRunning(ddbSetList.hostPath)) ddbSetList.hostOn = true;
		else if (ddbSetList.hostOn) closeSign = true;
	}
	DdbTrackReady = true;

	for (;;)
	{
		// ������������Ƿ����
		if (ddbSetList.mode == 1 || (ddbSetList.mode == 0 && ddbSetList.restartHost))
		{
			if (_waccess(ddbSetList.hostPath.c_str(), 0) == -1 && ddbSetList.hostPath != L"CommissioningTest")
				closeSign = true;
		}
		// ������������Ƿ�������
		if (ddbSetList.mode != 0)
		{
			if (isProcessRunning(ddbSetList.hostPath)) ddbSetList.hostOn = true;
			else if (ddbSetList.hostOn) closeSign = true;
		}

		this_thread::sleep_for(chrono::milliseconds(500));
	}
}