#include "IdtOther.h"

#include <tlhelp32.h>
#include <psapi.h>
#include "DdbConfiguration.h"

wstring GetCurrentExeDirectory()
{
	DWORD bufferSize = MAX_PATH;
	wstring buffer(bufferSize, L'\0');
	DWORD length = 0;

	while (true)
	{
		length = GetModuleFileNameW(NULL, &buffer[0], bufferSize);
		if (length == 0) return L"";
		else if (length < bufferSize)
		{
			buffer.resize(length);
			break;
		}
		else
		{
			bufferSize *= 2;
			buffer.resize(bufferSize, L'\0');
		}
	}

	filesystem::path fullPath(buffer);
	return fullPath.parent_path().wstring();
}
wstring GetCurrentExePath()
{
	DWORD bufferSize = MAX_PATH;
	wstring buffer(bufferSize, L'\0');
	DWORD length = 0;

	while (true)
	{
		length = GetModuleFileNameW(NULL, &buffer[0], bufferSize);
		if (length == 0) return L"";
		else if (length < bufferSize)
		{
			buffer.resize(length);
			break;
		}
		else
		{
			bufferSize *= 2;
			buffer.resize(bufferSize, L'\0');
		}
	}

	return buffer;
}
wstring GetCurrentExeName()
{
	DWORD bufferSize = MAX_PATH;
	wstring buffer(bufferSize, L'\0');
	DWORD length = 0;

	while (true)
	{
		length = GetModuleFileNameW(NULL, &buffer[0], bufferSize);
		if (length == 0) return L"";
		else if (length < bufferSize)
		{
			buffer.resize(length);
			break;
		}
		else
		{
			bufferSize *= 2;
			buffer.resize(bufferSize, L'\0');
		}
	}

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

// 程序进程状态获取
bool isProcessRunning(const wstring& processPath)
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
// 进程程序路径查询
int ProcessRunningCnt(const wstring& processPath)
{
	int ret = 0;

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
	{
		while (Process32Next(snapshot, &entry))
		{
			// 获取进程的完整路径
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

			// 比较路径是否相同
			if (wcslen(processFullPath) > 0 && wcscmp(processFullPath, processPath.c_str()) == 0) ret++;
		}
	}

	CloseHandle(snapshot);
	return ret;
}

// 占用文件（读写权限）
bool OccupyFile(HANDLE* hFile, const wstring& filePath)
{
	if (_waccess(filePath.c_str(), 0) == -1) return false;

	for (int time = 1; time <= 5; time++)
	{
		*hFile = CreateFileW(
			filePath.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,              // 不共享，独占访问
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);

		if (*hFile != INVALID_HANDLE_VALUE) break;
		else if (time >= 3) return false;

		this_thread::sleep_for(chrono::milliseconds(100));
	}

	if (*hFile != INVALID_HANDLE_VALUE) return true;
	return false;
}
// 释放文件
bool UnOccupyFile(HANDLE* hFile)
{
	if (*hFile != NULL)
	{
		CloseHandle(*hFile);
		return true;
	}
	return false;
}

bool DdbTrackReady;
void DdbTrack()
{
	// 检查宿主程序是否存在
	if (ddbSetList.mode == 1 || (ddbSetList.mode == 0 && ddbSetList.restartHost))
	{
		if (_waccess(ddbSetList.hostPath.c_str(), 0) == -1 && ddbSetList.hostPath != L"CommissioningTest")
			closeSign = true;
	}
	// 检查宿主程序是否在运行
	if (ddbSetList.mode != 0)
	{
		if (isProcessRunning(ddbSetList.hostPath)) ddbSetList.hostOn = true;
		else if (ddbSetList.hostOn) closeSign = true;
	}
	DdbTrackReady = true;

	for (;;)
	{
		// 检查宿主程序是否存在
		if (ddbSetList.mode == 1 || (ddbSetList.mode == 0 && ddbSetList.restartHost))
		{
			if (_waccess(ddbSetList.hostPath.c_str(), 0) == -1 && ddbSetList.hostPath != L"CommissioningTest")
				closeSign = true;
		}
		// 检查宿主程序是否在运行
		if (ddbSetList.mode != 0)
		{
			if (isProcessRunning(ddbSetList.hostPath)) ddbSetList.hostOn = true;
			else if (ddbSetList.hostOn) closeSign = true;
		}
		// 检查文件读取是否屡次失败
		if (occSErrorT > 3) closeSign = true;

		this_thread::sleep_for(chrono::milliseconds(500));
	}
}