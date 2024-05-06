#pragma once

#include "DdbMain.h"

#include "IdtText.h"

wstring GetCurrentExeDirectory();
wstring GetCurrentExePath();
wstring GetCurrentExeName();

bool isValidString(const wstring& str);