#include "DdbIntercept.h"

#include "DdbConfiguration.h"
#include "DdbMatch.h"

#include <string>

#include <tchar.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

// 拦截窗口列表
unordered_map<InterceptObjectEnum, WindowUnionClass> windowUnionList;

// 检测对象列表
vector<pair<WindowSearchStruct, DetectObjectEnum>> detectObjectList;
// 检测对象找到
unordered_map<DetectObjectEnum, bool> detectObjectFoundMap;

// 发现拦截窗口列表
vector<pair<WindowSearchStruct*, HWND>> foundInterceptWindows;

BOOL CALLBACK EnumWindowsCallback(HWND inquiryHwnd, LPARAM lParam)
{
	EnumChildWindows(inquiryHwnd, EnumWindowsCallback, lParam);

	for (auto i : magic_enum::enum_values<InterceptObjectEnum>() | views::take(3))
	{
		for (auto sw : windowUnionList[i].windows)
		{
			if (sw.windowTitle.enable)
			{
				unique_ptr<TCHAR[]> windowTitleBuffer(new TCHAR[1024]);
				GetWindowText(inquiryHwnd, windowTitleBuffer.get(), 1024);
				auto windowTitle = wstring(windowTitleBuffer.get());

				if (!RegexMatch(sw.windowTitle.windowTitle, windowTitle))
				{
					continue;
				}
			}
			if (sw.className.enable)
			{
				unique_ptr<TCHAR[]> classNameBuffer(new TCHAR[1024]);
				GetClassName(inquiryHwnd, classNameBuffer.get(), 1024);
				auto className = wstring(classNameBuffer.get());

				if (!RegexMatch(sw.className.className, className))
				{
					continue;
				}
			}
			if (sw.style.enable)
			{
				if (sw.style.matchType == StyleMatchTypeEnum::Subset)
				{
					if ((GetWindowLong(inquiryHwnd, GWL_STYLE) & sw.style.style) != sw.style.style)
						continue;
				}
				else // Exact
				{
					if (GetWindowLong(inquiryHwnd, GWL_STYLE) != sw.style.style)
						continue;
				}
			}
			if (sw.processName.enable)
			{
				DWORD processId = 0;
				GetWindowThreadProcessId(inquiryHwnd, &processId);
				wstring processName = L"";
				if (processId != 0)
				{
					// 尝试打开进程句柄
					HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);

					if (hProcess)
					{
						wchar_t fullPath[MAX_PATH];
						DWORD size = MAX_PATH;

						// 获取完整路径 (Win7+)
						if (QueryFullProcessImageNameW(hProcess, 0, fullPath, &size)) {
							// 使用 filesystem 获取文件名部分（例如从 C:\A\B.exe 提取出 B.exe）
							processName = std::filesystem::path(fullPath).filename().wstring();
						}
						CloseHandle(hProcess);
					}
				}

				if (sw.processName.processName != processName)
				{
					continue;
				}
			}
			if (sw.size.enable)
			{
				RECT rect{};
				HRESULT hr = DwmGetWindowAttribute(inquiryHwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));
				if (FAILED(hr) || (rect.right - rect.left == 0))
				{
					GetWindowRect(inquiryHwnd, &rect);
				}

				int twidth = rect.right - rect.left;
				int theight = rect.bottom - rect.top;

				// 定义一个小的容差范围，防止缩放产生的 1-2 像素误差
				const int pixelTolerance = 2;

				if (sw.size.MatchType == SizeMatchTypeEnum::Exact)
				{
					if (twidth != sw.size.width || theight != sw.size.height)
						continue;
				}
				else if (sw.size.MatchType == SizeMatchTypeEnum::Scale)
				{
					// 比例匹配：除了比例一致，还要确保窗口不是太小（避免除以0）
					if (sw.size.width == 0 || sw.size.height == 0) continue;

					float widthRatio = static_cast<float>(twidth) / static_cast<float>(sw.size.width);
					float heightRatio = static_cast<float>(theight) / static_cast<float>(sw.size.height);

					if (abs(widthRatio - heightRatio) > 0.03f)
						continue;
				}
				else // DPIScale
				{
					// 3. 获取 DPI
					HDC hdc = GetDC(inquiryHwnd); // 最好获取目标窗口所在 DC 的 DPI
					int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
					int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
					ReleaseDC(inquiryHwnd, hdc);

					float scaleX = dpiX / 96.0f;
					float scaleY = dpiY / 96.0f;

					// 计算理论上的缩放后尺寸
					float targetW = static_cast<float>(sw.size.width) * scaleX;
					float targetH = static_cast<float>(sw.size.height) * scaleY;

					// 判断：由于缩放舍入（如 1.25, 1.5），允许 ±2 像素误差
					if (abs(targetW - twidth) > (float)pixelTolerance ||
						abs(targetH - theight) > (float)pixelTolerance)
						continue;
				}
			}

			// Successfully matched
			cerr << "Find " + string(magic_enum::enum_name(i)) << " " << (int)inquiryHwnd << endl;

			foundInterceptWindows.push_back({ &sw, inquiryHwnd });
		}
	}

	return TRUE;
}
bool DdbIntercept()
{
	bool hasIntercept = false;

	foundInterceptWindows.clear();
	EnumWindows(EnumWindowsCallback, 0);

	for (auto fw : foundInterceptWindows)
	{
		auto hwnd = fw.second;
		auto interceptType = fw.first->interceptType;

		if (interceptType == InterceptTypeEnum::Close)
		{
			if (IsWindow(hwnd))
			{
				hasIntercept = true;
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}
		}
		if (interceptType == InterceptTypeEnum::Minimize)
		{
			if (!IsIconic(hwnd))
			{
				hasIntercept = true;
				ShowWindow(hwnd, SW_MINIMIZE);
			}
		}
		if (interceptType == InterceptTypeEnum::Hide)
		{
			vector<HWND> targets;
			targets.push_back(hwnd);

			// 1. 寻找被 hwnd 拥有的顶层窗口 (如独立的 Popup 弹窗)
			// 这些窗口在 Win32 层级上不是 Child，但逻辑上属于 hwnd
			EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
				{
					auto& list = *reinterpret_cast<vector<HWND>*>(lParam);
					if (GetWindow(hwnd, GW_OWNER) == list.front())
					{
						list.push_back(hwnd);
					}
					return TRUE;
				}, reinterpret_cast<LPARAM>(&targets));

			// 2. 递归查找所有子窗口 (真正的 Child 控件)
			// 我们需要对当前列表中的每一个窗口都执行子窗口查找
			size_t currentSize = targets.size();
			for (size_t i = 0; i < currentSize; i++)
			{
				EnumChildWindows(targets[i], [](HWND child, LPARAM lParam) -> BOOL
					{
						reinterpret_cast<vector<HWND>*>(lParam)->push_back(child);
						return TRUE;
					}, reinterpret_cast<LPARAM>(&targets));
			}

			// 3. 排序、去重并执行隐藏
			ranges::sort(targets);
			auto [first, last] = ranges::unique(targets);
			targets.erase(first, last);

			ranges::for_each(targets, [&](HWND h)
				{
					if (IsWindowVisible(h))
					{
						ShowWindow(h, SW_HIDE);

						hasIntercept = true;
						cerr << "Hide " << (int)h << endl;
					}
				});
		}
		if (interceptType == InterceptTypeEnum::Move)
		{
			RECT rect{};
			GetWindowRect(hwnd, &rect);
			int x = rect.left;
			int y = rect.top;

			if (x != -32000 || y != -32000)
			{
				fw.first->autoRecover.prevX = x;
				fw.first->autoRecover.prevY = y;

				hasIntercept = true;
				SetWindowPos(hwnd, NULL, -32000, -32000, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
			}
		}
	}

	/*
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
	}*/

	return hasIntercept;
}