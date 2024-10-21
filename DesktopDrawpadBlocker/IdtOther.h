#pragma once

#include "DdbMain.h"

#include "IdtText.h"

wstring GetCurrentExeDirectory();
wstring GetCurrentExePath();
wstring GetCurrentExeName();

bool isValidString(const wstring& str);

bool isProcessRunning(const std::wstring& processPath);
int ProcessRunningCnt(const std::wstring& processPath);

bool OccupyFile(HANDLE* hFile, const wstring& filePath);
bool UnOccupyFile(HANDLE* hFile);

extern bool DdbTrackReady;
void DdbTrack();