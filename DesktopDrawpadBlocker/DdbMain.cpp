/*
 * @file		DdbMain.cpp
 * @brief		DesktopDrawpadBlocker 项目中心源文件
 * @note		用于初始化 DesktopDrawpadBlocker 并调用相关模块
 *
 * @envir		VisualStudio 2022 | MSVC 143
 * @site		https://github.com/Alan-CRL/DesktopDrawpadBlocker
 *
 * @author		Alan-CRL
 * @qq			2685549821
 * @email		alan-crl@foxmail.com
 *
 * @description 项目文件中 Ddb 开头为本项目文件，Idt 开头为 智绘教Inkeys 项目引用（https://github.com/Alan-CRL/Inkeys）
*/

#include "DdbMain.h"

#include <sstream>
#include "DdbConfiguration.h"
#include "DdbIntercept.h"
#include "IdtGuid.h"
#include "IdtOther.h"
#include "IdtText.h"

#include <fstream>

wstring buildTime = __DATE__ L" " __TIME__;		//构建时间
wstring editionDate = L"20251128a";				//发布版本

wstring userid;									//用户ID
wstring globalPath;								//程序根路径

#ifndef DDB_RELEASE
void Test()
{
	MessageBoxW(NULL, L"标记处", L"标记", MB_OK | MB_SYSTEMMODAL);
}
void Testb(bool t)
{
	MessageBoxW(NULL, t ? L"true" : L"false", L"真否标记", MB_OK | MB_SYSTEMMODAL);
}
void Testi(long long t)
{
	MessageBoxW(NULL, to_wstring(t).c_str(), L"数值标记", MB_OK | MB_SYSTEMMODAL);
}
void Testw(wstring t)
{
	MessageBoxW(NULL, t.c_str(), L"字符标记", MB_OK | MB_SYSTEMMODAL);
}
void Testa(string t)
{
	MessageBoxW(NULL, utf8ToUtf16(t).c_str(), L"字符标记", MB_OK | MB_SYSTEMMODAL);
}
#endif

bool closeSign;

int WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR lpCmdLine, int /*nCmdShow*/)
{
	// 防止重复启动
	{
		if (ProcessRunningCnt(GetCurrentExePath()) > 1) return 0;
	}

	// 路径预处理
	{
		globalPath = (GetCurrentExeDirectory() + L"\\");
	}
	// 用户ID获取
	{
		userid = utf8ToUtf16(getDeviceGUID());
		if (userid.empty() || !isValidString(userid)) userid = L"IDError";
	}

	// 拦截配置初始化
	{
		// SeewoWhiteboard3Floating
		{
			// 希沃白板3 桌面悬浮窗
			WindowSearchStruct ws;
			{
				ws.windowTitle.enable = true;
				ws.windowTitle.windowTitle = LR"(Note)";
			}
			{
				ws.className.enable = true;
				ws.className.className = LR"(HwndWrapper\[EasiNote.exe;;[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\])";
			}
			{
				ws.style.enable = true;
				ws.style.style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_THICKFRAME | WS_GROUP | WS_TABSTOP | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
			}
			{
				ws.exStyle.enable = true;
				ws.exStyle.exStyle = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT | WS_EX_STATICEDGE | WS_EX_APPWINDOW | WS_EX_LAYERED | WS_EX_COMPOSITED;
			}
			{
				ws.processName.enable = true;
				ws.processName.processName = L"EasiNote.exe";
			}
			{
				ws.size.enable = true;
				ws.size.width = GetSystemMetrics(SM_CXSCREEN);
				ws.size.height = GetSystemMetrics(SM_CYSCREEN);
				ws.size.MatchType = SizeMatchTypeEnum::Exact;
			}
			windowUnionList[InterceptObjectEnum::SeewoWhiteboard3Floating].windows.push_back(ws);
		}
		// SeewoWhiteboard5Floating
		{
			// 希沃白板5 桌面悬浮窗
			WindowSearchStruct ws;
			{
				ws.className.enable = true;
				ws.className.className = LR"(HwndWrapper\[EasiNote;;[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\])";
			}
			{
				ws.style.enable = true;
				ws.style.style = 369623040;
			}
			{
				ws.size.enable = true;
				ws.size.width = 550;
				ws.size.height = 200;
				ws.size.MatchType = SizeMatchTypeEnum::DPIScale;
			}
			windowUnionList[InterceptObjectEnum::SeewoWhiteboard5Floating].windows.push_back(ws);
		}
		// SeewoWhiteboard5CFloating
		{
			// 希沃白板5C 桌面悬浮窗
			WindowSearchStruct ws;
			{
				ws.windowTitle.enable = true;
				ws.windowTitle.windowTitle = LR"()";
			}
			{
				ws.className.enable = true;
				ws.className.className = LR"(HwndWrapper\[EasiNote5C;;[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\])";
			}
			{
				ws.style.enable = true;
				ws.style.style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
			}
			{
				ws.exStyle.enable = true;
				ws.exStyle.exStyle = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_LAYERED | WS_EX_COMPOSITED;
			}
			{
				ws.size.enable = true;
				ws.size.width = 550;
				ws.size.height = 200;
				ws.size.MatchType = SizeMatchTypeEnum::DPIScale;
			}
			windowUnionList[InterceptObjectEnum::SeewoWhiteboard5CFloating].windows.push_back(ws);
		}
	}

	/*
		// SeewoPincoSideBarFloating
		{
			// 希沃品课教师端侧栏悬浮窗
			WindowSearchStruct ws;
			ws.hasWindowTitle = true;
			ws.windowTitle = L"希沃品课——appBar";
			ws.hasClassName = true;
			ws.className = L"Chrome_WidgetWin_1";
			ws.hasStyle = true;
			ws.style = WS_VISIBLE | WS_CLIPSIBLINGS | WS_GROUP | WS_MINIMIZEBOX;
			windowUnionList[InterceptObjectEnum::SeewoPincoSideBarFloating].windows.push_back(ws);
		}
		// SeewoPincoDrawingFloating
		{
			{
				// 希沃品课教师端桌面悬浮窗（包括PPT控件）
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"希沃品课——integration";
				ws.hasClassName = true;
				ws.className = L"Chrome_WidgetWin_1";
				ws.hasStyle = true;
				ws.style = 335675392;
				windowUnionList[InterceptObjectEnum::SeewoPincoDrawingFloating].windows.push_back(ws);
			}
			{
				// 希沃品课教师端桌面画板
				WindowSearchStruct ws;
				ws.hasClassName = true;
				ws.className = LR"(HwndWrapper\[BoardService;;[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\])";
				ws.hasStyle = true;
				ws.style = 369623040;
				ws.hasWidthHeight = true;
				ws.width = GetSystemMetrics(SM_CXSCREEN);
				ws.height = GetSystemMetrics(SM_CYSCREEN);
				windowUnionList[InterceptObjectEnum::SeewoPincoDrawingFloating].windows.push_back(ws);
			}
		}
		// SeewoPPTFloating
		{
			// 希沃PPT小工具
			WindowSearchStruct ws;
			ws.hasClassName = true;
			ws.className = LR"(HwndWrapper\[PPTService.exe;;[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\])";
			ws.hasStyle = true;
			ws.style = 369623040;
			windowUnionList[InterceptObjectEnum::SeewoPPTFloating].windows.push_back(ws);
		}
		// AiClassFloating
		{
			// AiClass 桌面悬浮窗
			WindowSearchStruct ws;
			ws.hasClassName = true;
			ws.className = L"UIWndTransparent";
			ws.hasWindowTitle = true;
			ws.windowTitle = L"TransparentWindow";
			ws.hasStyle = true;
			ws.style = -2080374784;
			windowUnionList[InterceptObjectEnum::AiClassFloating].windows.push_back(ws);
		}
		// HiteAnnotationFloating
		{
			// 鸿合屏幕书写
			WindowSearchStruct ws;
			ws.hasWindowTitle = true;
			ws.windowTitle = L"HiteAnnotation";
			ws.hasClassName = true;
			ws.className = L"Qt5QWindowToolSaveBits";
			ws.hasStyle = true;
			ws.style = -1778384896;
			windowUnionList[InterceptObjectEnum::HiteAnnotationFloating].windows.push_back(ws);
		}
		// ChangYanFloating
		{
			// 畅言智慧课堂 主栏悬浮窗
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"ifly";
				ws.hasClassName = true;
				ws.className = L"Qt5QWindowOwnDCIcon"; // 有报告指出可能变为 Qt5QWindowDCIcon
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
				ws.interceptType = InterceptTypeEnum::Hide;
				windowUnionList[InterceptObjectEnum::ChangYanFloating].windows.push_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"画笔设置";
				ws.hasClassName = true;
				ws.className = L"Qt5QWindowOwnDCIcon";
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
				ws.interceptType = InterceptTypeEnum::Hide;
				windowUnionList[InterceptObjectEnum::ChangYanFloating].windows.push_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"滑动清除";
				ws.hasClassName = true;
				ws.className = L"Qt5QWindowOwnDCIcon";
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
				ws.interceptType = InterceptTypeEnum::Hide;
				windowUnionList[InterceptObjectEnum::ChangYanFloating].windows.push_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"互动";
				ws.hasClassName = true;
				ws.className = L"Qt5QWindowOwnDCIcon";
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
				ws.interceptType = InterceptTypeEnum::Hide;
				windowUnionList[InterceptObjectEnum::ChangYanFloating].windows.push_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"学科应用";
				ws.hasClassName = true;
				ws.className = L"Qt5QWindowOwnDCIcon";
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
				ws.interceptType = InterceptTypeEnum::Hide;
				windowUnionList[InterceptObjectEnum::ChangYanFloating].windows.push_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"管控";
				ws.hasClassName = true;
				ws.className = L"Qt5QWindowOwnDCIcon";
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
				ws.interceptType = InterceptTypeEnum::Hide;
				windowUnionList[InterceptObjectEnum::ChangYanFloating].windows.push_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"通用工具";
				ws.hasClassName = true;
				ws.className = L"Qt5QWindowOwnDCIcon";
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
				ws.interceptType = InterceptTypeEnum::Hide;
				windowUnionList[InterceptObjectEnum::ChangYanFloating].windows.push_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"SceneToolbar";
				ws.hasClassName = true;
				ws.className = L"Qt5QWindowOwnDCIcon";
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
				ws.interceptType = InterceptTypeEnum::Hide;
				windowUnionList[InterceptObjectEnum::ChangYanFloating].windows.push_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"DrawWindow";
				ws.hasClassName = true;
				ws.className = L"Qt5QWindowToolSaveBits";
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
				ws.interceptType = InterceptTypeEnum::Hide;
				windowUnionList[InterceptObjectEnum::ChangYanFloating].windows.push_back(ws);
			}
		}
		// ChangYanPptFloating
		{
			// 畅言智慧课堂 PPT悬浮窗
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"Exch";
				ws.hasClassName = true;
				ws.className = L"Qt5QWindowToolSaveBitsOwnDC";
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
				ws.interceptType = InterceptTypeEnum::Hide;
				windowUnionList[InterceptObjectEnum::ChangYanPptFloating].windows.push_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"PageCtl";
				ws.hasClassName = true;
				ws.className = L"Qt5QWindowToolSaveBitsOwnDC";
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
				ws.interceptType = InterceptTypeEnum::Hide;
				windowUnionList[InterceptObjectEnum::ChangYanPptFloating].windows.push_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"Goback";
				ws.hasClassName = true;
				ws.className = L"Qt5QWindowToolSaveBitsOwnDC";
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
				ws.interceptType = InterceptTypeEnum::Hide;
				windowUnionList[InterceptObjectEnum::ChangYanPptFloating].windows.push_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"Preview";
				ws.hasClassName = true;
				ws.className = L"Qt5QWindowToolSaveBitsOwnDC";
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
				ws.interceptType = InterceptTypeEnum::Hide;
				windowUnionList[InterceptObjectEnum::ChangYanPptFloating].windows.push_back(ws);
			}
		}
		// IntelligentClassFloating
		{
			{
				// 天喻教育云互动课堂 主栏悬浮窗
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"桌面小工具 - 互动课堂";
				ws.hasClassName = true;
				ws.className = LR"(HwndWrapper\[IntelligentClassApp.exe;;[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\])";
				ws.hasStyle = true;
				ws.style = WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_MAXIMIZE | WS_SYSMENU;
				windowUnionList[InterceptObjectEnum::IntelligentClassFloating].windows.push_back(ws);
			}
			{
				// 天喻教育云互动课堂 主栏悬浮窗 (disabled状态)
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"桌面小工具 - 互动课堂";
				ws.hasClassName = true;
				ws.className = LR"(HwndWrapper\[IntelligentClassApp.exe;;[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\])";
				ws.hasStyle = true;
				ws.style = WS_VISIBLE | WS_DISABLED | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_MAXIMIZE | WS_SYSMENU;
				windowUnionList[InterceptObjectEnum::IntelligentClassFloating].windows.push_back(ws);
			}
			{
				// 天喻教育云互动课堂 PPT悬浮窗（同天喻教育云教学助手）
				WindowSearchStruct ws;
				ws.hasClassName = true;
				ws.className = LR"(HwndWrapper\[IntelligentClass.Office.PowerPoint.vsto|vstolocal;VSTA_Main;[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\])";
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
				windowUnionList[InterceptObjectEnum::IntelligentClassFloating].windows.push_back(ws);
			}
		}
		// SeewoDesktopAnnotationFloating
		{
			// 希沃桌面 画笔悬浮窗
			WindowSearchStruct ws;
			ws.hasClassName = true;
			ws.className = LR"(HwndWrapper\[HwndWrapper[DesktopAnnotation.exe;;[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\])";
			ws.hasStyle = true;
			ws.style = WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
			windowUnionList[InterceptObjectEnum::SeewoDesktopAnnotationFloating].windows.push_back(ws);
		}
		// SeewoDesktopSideBarFloating
		{
			// 希沃桌面 侧栏悬浮窗
			WindowSearchStruct ws;
			ws.hasWindowTitle = true;
			ws.windowTitle = L"ResidentSideBar";
			ws.hasClassName = true;
			ws.className = LR"(HwndWrapper\[HwndWrapper[ResidentSideBar.exe;;[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\])";
			ws.hasStyle = true;
			ws.style = WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_MAXIMIZE | WS_SYSMENU;
			windowUnionList[InterceptObjectEnum::SeewoDesktopSideBarFloating].windows.push_back(ws);
		}
		// Iclass30Floating
		{
			// C30 智能课堂 画笔悬浮窗 + PPT 控件
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"draw_CTransDrawWnd";
				ws.hasClassName = true;
				ws.className = L"draw_CTransDrawWnd";
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_GROUP | WS_TABSTOP | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
				ws.interceptType = InterceptTypeEnum::Move;
				windowUnionList[InterceptObjectEnum::Iclass30Floating].windows.push_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"工具栏";
				ws.hasClassName = true;
				ws.className = L"toolview";
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
				ws.interceptType = InterceptTypeEnum::Move;
				windowUnionList[InterceptObjectEnum::Iclass30Floating].windows.push_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"PenStatWnd";
				ws.hasClassName = true;
				ws.className = L"PenStatWnd";
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
				ws.interceptType = InterceptTypeEnum::Move;
				windowUnionList[InterceptObjectEnum::Iclass30Floating].windows.push_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"EraserWnd";
				ws.hasClassName = true;
				ws.className = L"EraserWnd";
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
				ws.interceptType = InterceptTypeEnum::Move;
				windowUnionList[InterceptObjectEnum::Iclass30Floating].windows.push_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"更多";
				ws.hasClassName = true;
				ws.className = L"more_dlg";
				ws.hasStyle = true;
				ws.style = WS_CLIPSIBLINGS | WS_VISIBLE | WS_CLIPCHILDREN | WS_SYSMENU | WS_THICKFRAME | WS_GROUP | WS_TABSTOP | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
				ws.interceptType = InterceptTypeEnum::Move;
				windowUnionList[InterceptObjectEnum::Iclass30Floating].windows.push_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"左翻页";
				ws.hasClassName = true;
				ws.className = L"LeftPageCtrlView";
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
				ws.interceptType = InterceptTypeEnum::Move;
				windowUnionList[InterceptObjectEnum::Iclass30Floating].windows.push_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"右翻页";
				ws.hasClassName = true;
				ws.className = L"CPageCtrlView";
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
				ws.interceptType = InterceptTypeEnum::Move;
				windowUnionList[InterceptObjectEnum::Iclass30Floating].windows.push_back(ws);
			}
		}
		// Iclass30SidebarFloating
		{
			// C30 智能课堂 边栏悬浮窗
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"控制工具条";
				ws.hasClassName = true;
				ws.className = L"ctrltool";
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
				ws.interceptType = InterceptTypeEnum::Hide;
				windowUnionList[InterceptObjectEnum::Iclass30SidebarFloating].windows.push_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.hasWindowTitle = true;
				ws.windowTitle = L"浮动工具条";
				ws.hasClassName = true;
				ws.className = L"fold_ctrl";
				ws.hasStyle = true;
				ws.style = WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
				ws.interceptType = InterceptTypeEnum::Minimize;
				windowUnionList[InterceptObjectEnum::Iclass30SidebarFloating].windows.push_back(ws);
			}
		}
	}

	// 检测配置初始化
	{
		{
			WindowSearchStruct ws;
			ws.hasWindowTitle = true;
			ws.windowTitle = LR"畅言智慧课堂-白板.*";
			ws.hasClassName = true;
			ws.className = L"Qt5QWindowIcon";
			ws.hasStyle = true;
			ws.style = WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_GROUP | WS_TABSTOP | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
			ws.interceptType = InterceptTypeEnum::Detection;
			detectObjectList.push_back(make_pair(ws, DetectObjectEnum::ChangYanWhiteboard));
		}

		// C30 智能课堂 超级白板（检测）
		WindowSearch[27].hasWindowTitle = true;
		WindowSearch[27].windowTitle = L"超级白板1";
		WindowSearch[27].hasClassName = true;
		WindowSearch[27].className = L"wb_CWhiteBoardPageWnd";
		WindowSearch[27].hasStyle = true;
		WindowSearch[27].style = WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_GROUP | WS_TABSTOP | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
		WindowSearch[27].interceptType = InterceptTypeEnum::Detection;
	}

	*/

	// 配置文件初始化
	{
		wstring parameters;
		wstringstream wss(lpCmdLine);

		getline(wss, parameters, L'-');
		getline(wss, parameters, L' ');

		if (parameters == L"startup" || _waccess((globalPath + L"start_up.signal").c_str(), 0) == 0)
		{
			if (_waccess((globalPath + L"interaction_configuration.json").c_str(), 0) == -1)
			{
				DdbWriteSetting(true);
				return 0;
			}
		}
		else if (!ConfigurationChange() || CloseSoftware())
		{
			DdbWriteSetting(true);
			return 0;
		}

		DdbReadSetting();
		DdbWriteSetting();
	}
	// 程序模式初始化
	{
		if (ddbSetList.mode == 1 && !isProcessRunning(ddbSetList.hostPath))
		{
			DdbWriteSetting(true);
			return 0;
		}

		thread ddbTrackThread(DdbTrack);
		ddbTrackThread.detach();

		while (!DdbTrackReady) this_thread::sleep_for(chrono::milliseconds(10));
	}

	// 开始拦截悬浮窗
	for (; !closeSign; this_thread::sleep_for(chrono::milliseconds(ddbSetList.sleepTime)))
	{
		// 查询程序是否需要关闭
		if (CloseSoftware()) break;

		// 查询配置文件是否修改
		if (ConfigurationChange())
		{
			DdbReadSetting();
			DdbWriteSetting();
		}

		// 拦截窗口
		bool res = DdbIntercept();

		// 扩展功能：重启宿主程序
		if (res && ddbSetList.mode == 0 && ddbSetList.restartHost && !isProcessRunning(ddbSetList.hostPath))
		{
			if (ddbSetList.hostPath != L"CommissioningTest" && _waccess(ddbSetList.hostPath.c_str(), 0) == 0)
			{
				// 重启宿主程序
				ShellExecuteW(NULL, NULL, ddbSetList.hostPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
			}
		}
	}

	DdbWriteSetting(true);
	return 0;
}