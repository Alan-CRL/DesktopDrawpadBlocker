#include "DdbIntercept.h"

#include "DdbConfiguration.h"

#include <tchar.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

WindowSearchStruct WindowSearch[10];
int WindowSearchSize;

BOOL CALLBACK EnumWindowsCallback(HWND inquiryHwnd, LPARAM lParam)
{
	EnumChildWindows(inquiryHwnd, EnumWindowsCallback, lParam);

	int foundCnt = 0;
	for (int i = 0; i < WindowSearchSize; i++)
	{
		if (WindowSearch[i].hasClassName)
		{
			TCHAR classNameBuffer[1024];
			GetClassName(inquiryHwnd, classNameBuffer, 1024);
			if (_tcsstr(classNameBuffer, WindowSearch[i].className.c_str()) == NULL)
				continue;
		}
		if (WindowSearch[i].hasWindowTitle)
		{
			TCHAR windowTitleBuffer[1024];
			GetWindowText(inquiryHwnd, windowTitleBuffer, 1024);
			if (_tcsstr(windowTitleBuffer, WindowSearch[i].windowTitle.c_str()) == NULL)
				continue;
		}

		if (WindowSearch[i].hasStyle)
		{
			if (GetWindowLong(inquiryHwnd, GWL_STYLE) != WindowSearch[i].style)
				continue;
		}
		if (WindowSearch[i].hasWidthHeight)
		{
			RECT rect{};
			DwmGetWindowAttribute(inquiryHwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));
			int twidth = rect.right - rect.left, thwight = rect.bottom - rect.top;

			HDC hdc = GetDC(NULL);
			int horizontalDPI = GetDeviceCaps(hdc, LOGPIXELSX);
			int verticalDPI = GetDeviceCaps(hdc, LOGPIXELSY);
			ReleaseDC(NULL, hdc);
			float scale = (horizontalDPI + verticalDPI) / 2.0f / 96.0f;

			if (abs(WindowSearch[i].width * scale - twidth) > 1 || abs(WindowSearch[i].height * scale - thwight) > 1)
				continue;
		}

		WindowSearch[i].outHwnd = inquiryHwnd;
		WindowSearch[i].foundHwnd = true;
	}

	for (int i = 0; i < WindowSearchSize; i++)
	{
		if (WindowSearch[i].foundHwnd)
			foundCnt++;
	}

	if (foundCnt == WindowSearchSize) return FALSE;
	return TRUE;
}

void DdbIntercept()
{
	EnumWindows(EnumWindowsCallback, 0);

	for (int i = 0; i < WindowSearchSize; i++)
	{
		if (setList.InterceptWindow[i] && WindowSearch[i].foundHwnd)
		{
			PostMessage(WindowSearch[i].outHwnd, WM_CLOSE, 0, 0);
			WindowSearch[i].foundHwnd = false;
		}
	}
}