#include "DdbIntercept.h"

#include "DdbConfiguration.h"

#include <string>

#include <tchar.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

WindowSearchStruct WindowSearch[40];
int WindowSearchSize;

BOOL CALLBACK EnumWindowsCallback(HWND inquiryHwnd, LPARAM lParam)
{
	EnumChildWindows(inquiryHwnd, EnumWindowsCallback, lParam);

	int foundCnt = 0;
	for (int i = 0; i < WindowSearchSize; i++)
	{
		if (WindowSearch[i].hasClassName)
		{
			unique_ptr<TCHAR[]> classNameBuffer(new TCHAR[1024]);
			GetClassName(inquiryHwnd, classNameBuffer.get(), 1024);
			if (_tcsstr(classNameBuffer.get(), WindowSearch[i].className.c_str()) == NULL)
				continue;
		}
		if (WindowSearch[i].hasWindowTitle)
		{
			unique_ptr<TCHAR[]> windowTitleBuffer(new TCHAR[1024]);
			GetWindowText(inquiryHwnd, windowTitleBuffer.get(), 1024);
			if (_tcsstr(windowTitleBuffer.get(), WindowSearch[i].windowTitle.c_str()) == NULL)
				continue;
		}
		if (WindowSearch[i].hasStyle)
		{
			if (9 <= i && i <= 21)
			{
				//Testi(i);
				if ((GetWindowLong(inquiryHwnd, GWL_STYLE) & WindowSearch[i].style) != WindowSearch[i].style)
					continue;

				//Testi(i);
				// GetWindowLong(inquiryHwnd, GWL_STYLE) != WindowSearch[i].style 修改一下这个，我现在判断的是，WindowSearch[i].style 是不是这个窗口的子集，类似 WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN 就是 WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN 的 true
			}
			else
			{
				if (GetWindowLong(inquiryHwnd, GWL_STYLE) != WindowSearch[i].style)
					continue;
			}
		}
		if (WindowSearch[i].hasWidthHeight)
		{
			RECT rect{};
			DwmGetWindowAttribute(inquiryHwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));
			int twidth = rect.right - rect.left, thwight = rect.bottom - rect.top;

			if (WindowSearch[i].width != twidth || WindowSearch[i].height != thwight)
			{
				HDC hdc = GetDC(NULL);
				int horizontalDPI = GetDeviceCaps(hdc, LOGPIXELSX);
				int verticalDPI = GetDeviceCaps(hdc, LOGPIXELSY);
				ReleaseDC(NULL, hdc);
				float scale = (horizontalDPI + verticalDPI) / 2.0f / 96.0f;

				if (abs(WindowSearch[i].width * scale - twidth) > 1 || abs(WindowSearch[i].height * scale - thwight) > 1)
					continue;
			}
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

struct
{
	bool detection27 = false; // C30 白板
	bool detection33 = false; // 畅言 白板
}testStruct;

bool DdbIntercept()
{
	bool ret = false;

	EnumWindows(EnumWindowsCallback, 0);

	{
		testStruct.detection27 = false;
		testStruct.detection33 = false;
	}
	for (int i = 0; i < WindowSearchSize; i++)
	{
		if (ddbSetList.InterceptWindow[i] && WindowSearch[i].foundHwnd)
		{
			ret = true;
			WindowSearch[i].foundHwnd = false;

			if (WindowSearch[i].interceptType == InterceptTypeEnum::Detection)
			{
				if (i == 27) testStruct.detection27 = true;
				if (i == 0 || i == 1)
				{
					//Testi(123);
					testStruct.detection33 = true;
				}
			}

			if (WindowSearch[i].interceptType == InterceptTypeEnum::Close)
			{
				if (IsWindow(WindowSearch[i].outHwnd))
				{
					PostMessage(WindowSearch[i].outHwnd, WM_CLOSE, 0, 0);
				}
			}
			if (WindowSearch[i].interceptType == InterceptTypeEnum::Minimize)
			{
				if (!IsIconic(WindowSearch[i].outHwnd))
				{
					ShowWindow(WindowSearch[i].outHwnd, SW_MINIMIZE);
				}
			}
			if (WindowSearch[i].interceptType == InterceptTypeEnum::Hide)
			{
				if ((9 <= i && i <= 17))
				{
					if (testStruct.detection33)
					{
						if (!IsWindowVisible(WindowSearch[i].outHwnd))
						{
							ShowWindow(WindowSearch[i].outHwnd, SW_SHOWNOACTIVATE);
						}
					}
					else
					{
						if (IsWindowVisible(WindowSearch[i].outHwnd))
						{
							ShowWindow(WindowSearch[i].outHwnd, SW_HIDE);
						}
					}
				}
				else
				{
					// 常规拦截
					if (IsWindowVisible(WindowSearch[i].outHwnd))
					{
						ShowWindow(WindowSearch[i].outHwnd, SW_HIDE);
					}
				}
			}
			if (WindowSearch[i].interceptType == InterceptTypeEnum::Move)
			{
				RECT rect{};
				GetWindowRect(WindowSearch[i].outHwnd, &rect);
				int x = rect.left;
				int y = rect.top;

				if ((28 <= i && i <= 32) || (36 <= i && i <= 37))
				{
					if (testStruct.detection27)
					{
						if (x == -32000 && y == -32000)
						{
							SetWindowPos(WindowSearch[i].outHwnd, NULL, WindowSearch[i].prevX, WindowSearch[i].prevY, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
						}
						else
						{
							WindowSearch[i].prevX = x;
							WindowSearch[i].prevY = y;
						}
					}
					else
					{
						if (x != -32000 || y != -32000)
						{
							WindowSearch[i].prevX = x;
							WindowSearch[i].prevY = y;

							SetWindowPos(WindowSearch[i].outHwnd, NULL, -32000, -32000, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
						}
					}
				}
				else
				{
					if (x != -32000 || y != -32000)
					{
						WindowSearch[i].prevX = x;
						WindowSearch[i].prevY = y;

						SetWindowPos(WindowSearch[i].outHwnd, NULL, -32000, -32000, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
					}
				}
			}
		}
	}

	return ret;
}