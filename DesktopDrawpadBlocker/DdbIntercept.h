﻿#pragma once

#include "DdbMain.h"

struct WindowSearchStruct
{
	bool hasClassName;
	wstring className;

	bool hasWindowTitle;
	wstring windowTitle;

	bool hasStyle;
	LONG style;

	bool hasWidthHeight;
	int width;
	int height;

	bool foundHwnd = false;
	HWND outHwnd = nullptr;
};
extern WindowSearchStruct WindowSearch[30];
extern int WindowSearchSize;

bool DdbIntercept();