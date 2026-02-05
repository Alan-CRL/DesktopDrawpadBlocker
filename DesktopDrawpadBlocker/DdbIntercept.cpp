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
unordered_map<DetectObjectEnum, bool> foundDetectWindows;

// 发现拦截窗口列表
vector<pair<WindowSearchStruct*, HWND>> foundInterceptWindows;
// 自动恢复位置记录
libcuckoo::cuckoohash_map<HWND, pair<int, int>> autoRecoverPositions;

// 窗口钩子
libcuckoo::cuckoohash_map<HWND, InterceptTypeEnum> windowTracker;
void CALLBACK WindowTrackerProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
	// 1. 过滤：只处理窗口对象本身的消息，不处理窗口内的按钮、菜单等
	if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF) return;

	// 2. 过滤：只处理我们锁定的那个窗口
	InterceptTypeEnum interceptType;
	if (!windowTracker.find(hwnd, interceptType)) return;
	if (event == EVENT_OBJECT_DESTROY)
	{
		windowTracker.erase(hwnd);
		return;
	}

	if (interceptType == InterceptTypeEnum::Minimize && event == EVENT_SYSTEM_MINIMIZESTART)
	{
		cerr << "MINIMIZESTART call: " << (int)hwnd << endl;

		thread(DdbIntercept).detach();
		return;
	}
	if (interceptType == InterceptTypeEnum::Hide && event == EVENT_OBJECT_HIDE)
	{
		cerr << "HIDE call: " << (int)hwnd << endl;

		thread(DdbIntercept).detach();
		return;
	}
	if (interceptType == InterceptTypeEnum::Move && event == EVENT_OBJECT_LOCATIONCHANGE)
	{
		RECT rect;
		GetWindowRect(hwnd, &rect);

		if (rect.left != -32000 || rect.top == -32000)
		{
			cerr << "LOCATIONCHANGE call: " << (int)hwnd << endl;
			thread(DdbIntercept).detach();
		}

		return;
	}
}
void WindowTrackerStart()
{
	// 钩子 1: 监控 销毁、隐藏、显示、位置改变
	// 范围从 0x8001 (DESTROY) 到 0x800B (LOCATIONCHANGE)
	HWINEVENTHOOK hHookObject = SetWinEventHook(
		EVENT_OBJECT_DESTROY,          // 0x8001
		EVENT_OBJECT_LOCATIONCHANGE,   // 0x800B
		nullptr,
		WindowTrackerProc,
		0, 0,
		WINEVENT_OUTOFCONTEXT
	);

	// 钩子 2: 监控 最小化开始
	// 因为它在系统常量区 (0x0016)，不在 0x8000 区间内
	HWINEVENTHOOK hHookSys = SetWinEventHook(
		EVENT_SYSTEM_MINIMIZESTART,
		EVENT_SYSTEM_MINIMIZESTART,
		nullptr,
		WindowTrackerProc,
		0, 0,
		WINEVENT_OUTOFCONTEXT
	);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// 清理
	UnhookWinEvent(hHookObject);
	UnhookWinEvent(hHookSys);

	return;
}

// 窗口查找
bool IsMatchWindow(WindowSearchStruct& sw, HWND inquiryHwnd)
{
	if (sw.windowTitle.enable)
	{
		unique_ptr<TCHAR[]> windowTitleBuffer(new TCHAR[1024]);
		GetWindowText(inquiryHwnd, windowTitleBuffer.get(), 1024);
		auto windowTitle = wstring(windowTitleBuffer.get());

		if (!RegexMatch(sw.windowTitle.windowTitle, windowTitle))
		{
			return false;
		}
	}
	if (sw.className.enable)
	{
		unique_ptr<TCHAR[]> classNameBuffer(new TCHAR[1024]);
		GetClassName(inquiryHwnd, classNameBuffer.get(), 1024);
		auto className = wstring(classNameBuffer.get());

		if (!RegexMatch(sw.className.className, className))
		{
			return false;
		}
	}
	if (sw.style.enable)
	{
		if (sw.style.matchType == StyleMatchTypeEnum::Subset)
		{
			if ((GetWindowLong(inquiryHwnd, GWL_STYLE) & sw.style.style) != sw.style.style)
				return false;
		}
		else // Exact
		{
			if (GetWindowLong(inquiryHwnd, GWL_STYLE) != sw.style.style)
				return false;
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
			return false;
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
				return false;
		}
		else if (sw.size.MatchType == SizeMatchTypeEnum::Scale)
		{
			// 比例匹配：除了比例一致，还要确保窗口不是太小（避免除以0）
			if (sw.size.width == 0 || sw.size.height == 0) return false;

			float widthRatio = static_cast<float>(twidth) / static_cast<float>(sw.size.width);
			float heightRatio = static_cast<float>(theight) / static_cast<float>(sw.size.height);

			if (abs(widthRatio - heightRatio) > 0.03f)
				return false;
		}
		else if (sw.size.MatchType == SizeMatchTypeEnum::DPIScale)
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
				return false;
		}
		else if (sw.size.MatchType == SizeMatchTypeEnum::FullScreen)
		{
			if (twidth != GetSystemMetrics(SM_CXSCREEN) || theight != GetSystemMetrics(SM_CYSCREEN))
				return false;
		}
		else if (sw.size.MatchType == SizeMatchTypeEnum::FullHeight)
		{
			if (theight != GetSystemMetrics(SM_CYSCREEN))
				return false;
		}
		else if (sw.size.MatchType == SizeMatchTypeEnum::FullWidth)
		{
			if (twidth != GetSystemMetrics(SM_CXSCREEN))
				return false;
		}
	}

	return true;
}
BOOL CALLBACK EnumWindowsCallback(HWND inquiryHwnd, LPARAM lParam)
{
	EnumChildWindows(inquiryHwnd, EnumWindowsCallback, lParam);

	// 检测窗口处理
	for (auto x : detectObjectList)
	{
		bool res = IsMatchWindow(x.first, inquiryHwnd);

		if (res)
		{
			cerr << "Find(Detection) " + string(magic_enum::enum_name(x.second)) << " " << (int)inquiryHwnd << endl;
			foundDetectWindows[x.second] = true;
		}
	}
	// 拦截窗口处理
	for (auto i : magic_enum::enum_values<InterceptObjectEnum>())
	{
		if (!windowUnionList[i].enable) continue;
		for (auto& sw : windowUnionList[i].windows)
		{
			bool res = IsMatchWindow(sw, inquiryHwnd);

			// Successfully matched

			if (res)
			{
				cerr << "Find " + string(magic_enum::enum_name(i)) << " " << (int)inquiryHwnd << endl;
				foundInterceptWindows.push_back({ &sw, inquiryHwnd });
			}
		}
	}

	return TRUE;
}
void CollectChild(HWND currentHwnd, set<HWND>& collectedWindows)
{
	// 递归查找所有子窗口 (Parent -> Child 关系)
	// EnumChildWindows 会遍历所有后代窗口，所以不需要在回调中再次递归。
	// 但为了代码结构的统一性和应对某些特殊情况，手动递归是更可靠的模式。
	// 这里我们使用一个递归的 lambda。
	function<void(HWND)> findChildren = [&](HWND parent)
		{
			EnumChildWindows(parent, [](HWND child, LPARAM lParam) -> BOOL
				{
					auto& recursive_func = *reinterpret_cast<function<void(HWND)>*>(lParam);
					recursive_func(child); // 对每个子窗口进行完整的递归查找
					return TRUE;
				}, reinterpret_cast<LPARAM>(&findChildren));
		};

	// 为当前窗口启动子窗口的递归查找
	EnumChildWindows(currentHwnd, [](HWND child, LPARAM lParam) -> BOOL
		{
			auto& windowsSet = *reinterpret_cast<set<HWND>*>(lParam);
			// 对每个直接子窗口，调用主递归函数
			if (IsWindow(child) && !windowsSet.count(child))
			{
				windowsSet.insert(child);
				CollectChild(child, windowsSet);
			}
			return TRUE;
		}, reinterpret_cast<LPARAM>(&collectedWindows));

	// 查找并递归处理此窗口拥有的其他顶层窗口 (Owner -> Owned 关系)
	pair<HWND, set<HWND>*> params(currentHwnd, &collectedWindows);
	// 我们需要遍历桌面上的所有窗口来找到它们
	EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
		{
			auto params = reinterpret_cast<pair<HWND, set<HWND>*>*>(lParam);
			HWND ownerHwnd = params->first;
			set<HWND>* windowsSet = params->second;

			if (GetWindow(hwnd, GW_OWNER) == ownerHwnd)
			{
				// 找到了一个被拥有的窗口，对它进行完整的递归查找
				if (IsWindow(hwnd) && !windowsSet->count(hwnd))
				{
					windowsSet->insert(hwnd);
					CollectChild(hwnd, *windowsSet);
				}
			}
			return TRUE;
		}, reinterpret_cast<LPARAM>(&params));
}

mutex interceptMutex;
bool DdbIntercept()
{
	unique_lock lock(interceptMutex, try_to_lock);
	if (!lock.owns_lock()) return false;

	bool hasIntercept = false;

	foundInterceptWindows.clear();
	foundDetectWindows.clear();

	EnumWindows(EnumWindowsCallback, 0);

	for (auto fw : foundInterceptWindows)
	{
		auto interceptType = fw.first->interceptType;
		if (!IsWindow(fw.second)) continue;

		set<HWND> targets;
		if (fw.first->interceptScope == InterceptScopeEnum::SelfAndChild || fw.first->interceptScope == InterceptScopeEnum::Self)
		{
			targets.insert(fw.second);
		}
		if (fw.first->interceptScope == InterceptScopeEnum::SelfAndChild || fw.first->interceptScope == InterceptScopeEnum::Child)
		{
			CollectChild(fw.second, targets);
		}

		for (HWND h : targets)
		{
			if (interceptType == InterceptTypeEnum::Close)
			{
				if (IsWindow(h))
				{
					hasIntercept = true;
					PostMessage(h, WM_CLOSE, 0, 0);
					// 需要特别注意的是，通过发送消息的方式关闭窗口，则会导致父/子窗口由于消息传递而关闭

					cerr << "Close " << (int)h << endl;
				}
				else cerr << "Close(Fail) " << (int)h << endl;
			}
			if (interceptType == InterceptTypeEnum::Minimize)
			{
				if (fw.first->windowTracker.enable)
				{
					windowTracker.insert_or_assign(h, InterceptTypeEnum::Minimize);
					cerr << "WindowTracker(Minimize) " << (int)h << endl;
				}

				if (!IsIconic(h))
				{
					hasIntercept = true;
					ShowWindow(h, SW_MINIMIZE);

					cerr << "Minimize " << (int)h << endl;
				}
				else cerr << "Minimize(Fail) " << (int)h << endl;
			}
			if (interceptType == InterceptTypeEnum::Hide)
			{
				if (fw.first->windowTracker.enable)
				{
					windowTracker.insert_or_assign(h, InterceptTypeEnum::Hide);
					cerr << "WindowTracker(Hide) " << (int)h << endl;
				}

				if (fw.first->autoRecover.enable && foundDetectWindows[fw.first->autoRecover.detectTarget] == true)
				{
					// 特殊情况恢复显示
					if (!IsWindowVisible(h))
					{
						hasIntercept = true;
						ShowWindow(h, SW_SHOWNOACTIVATE);

						cerr << "UnHide " << (int)h << endl;
					}
					else cerr << "UnHide(Fail) " << (int)h << endl;
				}
				else
				{
					// 常规拦截
					if (IsWindowVisible(h))
					{
						hasIntercept = true;
						ShowWindow(h, SW_HIDE);

						cerr << "Hide " << (int)h << endl;
					}
					else cerr << "Hide(Fail) " << (int)h << endl;
				}
			}
			if (interceptType == InterceptTypeEnum::Move)
			{
				RECT rect{};
				GetWindowRect(h, &rect);
				int x = rect.left;
				int y = rect.top;

				if (fw.first->windowTracker.enable)
				{
					windowTracker.insert_or_assign(h, InterceptTypeEnum::Move);
					cerr << "WindowTracker(Move) " << (int)h << endl;
				}

				if (fw.first->autoRecover.enable && foundDetectWindows[fw.first->autoRecover.detectTarget] == true)
				{
					// 特殊情况恢复显示
					pair<int, int> tmp;
					if (autoRecoverPositions.find(h, tmp))
					{
						int tx = tmp.first;
						int ty = tmp.second;

						if (x == -32000 && y == -32000)
						{
							SetWindowPos(h, NULL, tx, ty, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);

							cerr << "UnMove " << (int)h << endl;
						}
						else cerr << "UnMove(Fail) " << (int)h << endl;
					}
					else cerr << "UnMove(Fail2) " << (int)h << endl;
				}
				else
				{
					// 常规拦截
					if (x != -32000 || y != -32000)
					{
						autoRecoverPositions.insert_or_assign(h, make_pair(x, y));

						hasIntercept = true;
						SetWindowPos(h, NULL, -32000, -32000, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);

						cerr << "Move " << (int)h << endl;
					}
					else cerr << "Move(Fail) " << (int)h << endl;
				}
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