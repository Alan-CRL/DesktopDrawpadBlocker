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
			// 希沃白板3 桌面画笔悬浮窗
			WindowSearchStruct ws;
			ws.interceptType = InterceptTypeEnum::Hide;
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
				ws.processName.enable = true;
				ws.processName.processName = L"EasiNote.exe";
			}
			{
				ws.size.enable = true;
				ws.size.MatchType = SizeMatchTypeEnum::FullScreen;
			}
			windowUnionList[InterceptObjectEnum::SeewoWhiteboard3Floating].windows.emplace_back(ws);
		}
		// SeewoWhiteboard5Floating
		{
			// 希沃白板5 桌面画笔悬浮窗
			WindowSearchStruct ws;
			ws.interceptType = InterceptTypeEnum::Hide;
			{
				ws.windowTitle.enable = true;
				ws.windowTitle.windowTitle = L"";
			}
			{
				ws.className.enable = true;
				ws.className.className = LR"(HwndWrapper\[EasiNote;;[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\])";
			}
			{
				ws.style.enable = true;
				ws.style.style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
			}
			{
				ws.processName.enable = true;
				ws.processName.processName = L"EasiNote.exe";
			}
			{
				ws.size.enable = true;
				ws.size.MatchType = SizeMatchTypeEnum::FullScreen;
			}
			windowUnionList[InterceptObjectEnum::SeewoWhiteboard5Floating].windows.emplace_back(ws);
		}
		// SeewoWhiteboard5CFloating
		{
			// 希沃轻白板（5C） 桌面画笔悬浮窗
			{
				WindowSearchStruct ws;
				ws.interceptType = InterceptTypeEnum::Hide;
				{
					ws.windowTitle.enable = true;
					ws.windowTitle.windowTitle = L"";
				}
				{
					ws.className.enable = true;
					ws.className.className = LR"(HwndWrapper\[EasiNote5C;;[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\])";
				}
				{
					ws.style.enable = true;
					ws.style.style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
					ws.style.matchType = StyleMatchTypeEnum::Subset;
				}
				{
					ws.processName.enable = true;
					ws.processName.processName = L"EasiNote5C.exe";
				}
				{
					ws.size.enable = true;
					ws.size.MatchType = SizeMatchTypeEnum::FullScreen;
				}
				windowUnionList[InterceptObjectEnum::SeewoWhiteboard5CFloating].windows.emplace_back(ws);
			}
		}
		// SeewoPincoSideBarFloating
		{
			// 希沃品课教师端 侧栏悬浮窗
			{
				WindowSearchStruct ws;
				ws.interceptType = InterceptTypeEnum::Close;
				{
					ws.windowTitle.enable = true;
					ws.windowTitle.windowTitle = LR"(希沃品课——appBar)";
				}
				{
					ws.className.enable = true;
					ws.className.className = LR"(Chrome_WidgetWin_1)";
				}
				{
					ws.style.enable = true;
					ws.style.style = WS_CLIPSIBLINGS | WS_GROUP | WS_MINIMIZEBOX;
				}
				{
					ws.processName.enable = true;
					ws.processName.processName = L"seewoPincoTeacher.exe";
				}
				ws.interceptScope = InterceptScopeEnum::Self;
				windowUnionList[InterceptObjectEnum::SeewoPincoSideBarFloating].windows.emplace_back(ws);
			}
		}
		// SeewoPincoDrawingFloating
		{
			// 希沃品课教师端 桌面画笔悬浮窗（包括PPT控件）
			{
				WindowSearchStruct ws;
				ws.interceptType = InterceptTypeEnum::Close;
				{
					ws.windowTitle.enable = true;
					ws.windowTitle.windowTitle = LR"(希沃品课——integration)";
				}
				{
					ws.className.enable = true;
					ws.className.className = LR"(Chrome_WidgetWin_1)";
				}
				{
					ws.style.enable = true;
					ws.style.style = WS_CLIPSIBLINGS | WS_GROUP | WS_MINIMIZEBOX;
				}
				{
					ws.processName.enable = true;
					ws.processName.processName = L"seewoPincoTeacher.exe";
				}
				{
					ws.size.enable = true;
					ws.size.MatchType = SizeMatchTypeEnum::FullScreen;
				}
				windowUnionList[InterceptObjectEnum::SeewoPincoDrawingFloating].windows.emplace_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.interceptType = InterceptTypeEnum::Close;
				{
					ws.windowTitle.enable = true;
					ws.windowTitle.windowTitle = L"";
				}
				{
					ws.className.enable = true;
					ws.className.className = LR"(HwndWrapper\[BoardService;;[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\])";
				}
				{
					ws.style.enable = true;
					ws.style.style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
				}
				{
					ws.processName.enable = true;
					ws.processName.processName = L"BoardService.exe";
				}
				{
					ws.size.enable = true;
					ws.size.MatchType = SizeMatchTypeEnum::FullScreen;
				}
				windowUnionList[InterceptObjectEnum::SeewoPincoDrawingFloating].windows.emplace_back(ws);
			}
		}
		// SeewoPPTFloating
		{
			// 希沃PPT小工具
			{
				WindowSearchStruct ws;
				ws.interceptType = InterceptTypeEnum::Hide;
				{
					ws.windowTitle.enable = true;
					ws.windowTitle.windowTitle = L"";
				}
				{
					ws.className.enable = true;
					ws.className.className = LR"(HwndWrapper\[PPTService.exe;;[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\])";
				}
				{
					ws.style.enable = true;
					ws.style.style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
					ws.style.matchType = StyleMatchTypeEnum::Subset;
				}
				{
					ws.processName.enable = true;
					ws.processName.processName = L"PPTService.exe";
				}
				{
					ws.size.enable = true;
					ws.size.MatchType = SizeMatchTypeEnum::FullScreen;
				}
				windowUnionList[InterceptObjectEnum::SeewoPPTFloating].windows.emplace_back(ws);
			}
		}
		// SeewoIwbAssistantFloating
		{
			// 希沃课堂助手 PPT小工具
			{
				WindowSearchStruct ws;
				ws.interceptType = InterceptTypeEnum::Hide;
				{
					ws.windowTitle.enable = true;
					ws.windowTitle.windowTitle = L"";
				}
				{
					ws.className.enable = true;
					ws.className.className = LR"(HwndWrapper\[SeewoIwbAssistant;;[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\])";
				}
				{
					ws.style.enable = true;
					ws.style.style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
					ws.style.matchType = StyleMatchTypeEnum::Subset;
				}
				{
					ws.processName.enable = true;
					ws.processName.processName = L"SeewoIwbAssistant.exe";
				}
				{
					ws.size.enable = true;
					ws.size.MatchType = SizeMatchTypeEnum::FullScreen;
				}
				windowUnionList[InterceptObjectEnum::SeewoIwbAssistantFloating].windows.emplace_back(ws);
			}
		}
		// YiouBoardFloating
		{
			// 欧帝白板 桌面画笔悬浮窗（包括PPT控件）
			{
				WindowSearchStruct ws;
				ws.interceptType = InterceptTypeEnum::Hide;
				{
					ws.windowTitle.enable = true;
					ws.windowTitle.windowTitle = LR"(WinYODesktop)";
				}
				{
					ws.className.enable = true;
					ws.className.className = LR"(HwndWrapper\[WriteBoard.exe;;[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\])";
				}
				{
					ws.style.enable = true;
					ws.style.style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
					ws.style.matchType = StyleMatchTypeEnum::Subset;
				}
				{
					ws.processName.enable = true;
					ws.processName.processName = L"WriteBoard.exe";
				}
				{
					ws.size.enable = true;
					ws.size.width = 780;
					ws.size.height = 60;
					ws.size.MatchType = SizeMatchTypeEnum::DPIScale;
				}
				windowUnionList[InterceptObjectEnum::YiouBoardFloating].windows.emplace_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.interceptType = InterceptTypeEnum::Hide;
				{
					ws.windowTitle.enable = true;
					ws.windowTitle.windowTitle = LR"(WinDesktop)";
				}
				{
					ws.className.enable = true;
					ws.className.className = LR"(HwndWrapper\[WriteBoard.exe;;[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\])";
				}
				{
					ws.style.enable = true;
					ws.style.style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
					ws.style.matchType = StyleMatchTypeEnum::Subset;
				}
				{
					ws.processName.enable = true;
					ws.processName.processName = L"WriteBoard.exe";
				}
				{
					ws.size.enable = true;
					ws.size.MatchType = SizeMatchTypeEnum::FullScreen;
				}
				ws.interceptScope = InterceptScopeEnum::Self;
				windowUnionList[InterceptObjectEnum::YiouBoardFloating].windows.emplace_back(ws);
			}
		}
		// AiClassFloating
		{
			// AiClass 桌面画笔悬浮窗
			{
				WindowSearchStruct ws;
				ws.interceptType = InterceptTypeEnum::Hide;
				{
					ws.windowTitle.enable = true;
					ws.windowTitle.windowTitle = LR"(TransparentWindow)";
				}
				{
					ws.className.enable = true;
					ws.className.className = LR"(UIWndTransparent)";
				}
				{
					ws.style.enable = true;
					ws.style.style = WS_POPUP | WS_CLIPSIBLINGS;
					ws.style.matchType = StyleMatchTypeEnum::Subset;
				}
				{
					ws.processName.enable = true;
					ws.processName.processName = L"wisdom_class.exe";
				}
				{
					ws.size.enable = true;
					ws.size.MatchType = SizeMatchTypeEnum::FullScreen;
				}
				windowUnionList[InterceptObjectEnum::AiClassFloating].windows.emplace_back(ws);
			}
		}
		// ClassInXFloating
		{
			// ClassIn X 桌面画笔悬浮窗
			{
				WindowSearchStruct ws;
				ws.interceptType = InterceptTypeEnum::Hide;
				{
					ws.windowTitle.enable = true;
					ws.windowTitle.windowTitle = LR"(ClassIn X)";
				}
				{
					ws.className.enable = true;
					ws.className.className = LR"(Qt5151QWindowIcon)";
				}
				{
					ws.style.enable = true;
					ws.style.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
					ws.style.matchType = StyleMatchTypeEnum::Subset;
				}
				{
					ws.processName.enable = true;
					ws.processName.processName = L"ClassIn X.exe";
				}
				{
					ws.size.enable = true;
					ws.size.MatchType = SizeMatchTypeEnum::FullScreen;
				}
				windowUnionList[InterceptObjectEnum::ClassInXFloating].windows.emplace_back(ws);
			}
		}
		// IntelligentClassFloating
		{
			// 天喻教育云互动课堂 主栏悬浮窗
			{
				WindowSearchStruct ws;
				ws.interceptType = InterceptTypeEnum::Hide;
				{
					ws.windowTitle.enable = true;
					ws.windowTitle.windowTitle = LR"(桌面小工具 - 互动课堂)";
				}
				{
					ws.className.enable = true;
					ws.className.className = LR"(HwndWrapper\[IntelligentClassApp.exe;;[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\])";
				}
				{
					ws.style.enable = true;
					ws.style.style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_MAXIMIZE | WS_SYSMENU;
					ws.style.matchType = StyleMatchTypeEnum::Subset;
				}
				{
					ws.processName.enable = true;
					ws.processName.processName = L"IntelligentClassApp.exe";
				}
				{
					ws.size.enable = true;
					ws.size.MatchType = SizeMatchTypeEnum::FullScreen;
				}
				windowUnionList[InterceptObjectEnum::IntelligentClassFloating].windows.emplace_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.interceptType = InterceptTypeEnum::Hide;
				{
					ws.windowTitle.enable = true;
					ws.windowTitle.windowTitle = L"";
				}
				{
					ws.className.enable = true;
					ws.className.className = LR"(HwndWrapper\[IntelligentClass.Office.PowerPoint.vsto|vstolocal;VSTA_Main;[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}\])";
				}
				{
					ws.style.enable = true;
					ws.style.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
					ws.style.matchType = StyleMatchTypeEnum::Subset;
				}
				{
					ws.size.enable = true;
					ws.size.MatchType = SizeMatchTypeEnum::FullScreen;
				}
				windowUnionList[InterceptObjectEnum::IntelligentClassFloating].windows.emplace_back(ws);
			}
		}
		// ChangYanFloating
		{
			// 畅言智慧课堂4.0 桌面画笔悬浮窗（包括PPT控件）
			{
				WindowSearchStruct ws;
				ws.interceptType = InterceptTypeEnum::Hide;
				{
					ws.windowTitle.enable = true;
					ws.windowTitle.windowTitle = LR"(DrawWindow)";
				}
				{
					ws.className.enable = true;
					ws.className.className = LR"(Qt5QWindowToolSaveBits)";
				}
				{
					ws.style.enable = true;
					ws.style.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
					ws.style.matchType = StyleMatchTypeEnum::Subset;
				}
				{
					ws.processName.enable = true;
					ws.processName.processName = L"ifly.qzk.Toolbar.exe";
				}
				{
					ws.size.enable = true;
					ws.size.MatchType = SizeMatchTypeEnum::FullScreen;
				}
				windowUnionList[InterceptObjectEnum::ChangYanFloating].windows.emplace_back(ws);
			}
		}
		// ChangYan5Floating
		{
			// 畅言智慧课堂5.0 桌面画笔悬浮窗（包括PPT控件）
			{
				WindowSearchStruct ws;
				ws.interceptType = InterceptTypeEnum::Hide;
				{
					ws.windowTitle.enable = true;
					ws.windowTitle.windowTitle = LR"(DrawWindow)";
				}
				{
					ws.className.enable = true;
					ws.className.className = LR"(Qt5152QWindowIcon)";
				}
				{
					ws.style.enable = true;
					ws.style.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
					ws.style.matchType = StyleMatchTypeEnum::Subset;
				}
				{
					ws.processName.enable = true;
					ws.processName.processName = L"ifly.qzk.Toolbar.exe";
				}
				{
					ws.size.enable = true;
					ws.size.MatchType = SizeMatchTypeEnum::FullScreen;
				}
				windowUnionList[InterceptObjectEnum::ChangYan5Floating].windows.emplace_back(ws);
			}
		}
		// Iclass30Floating
		{
			// C30智能教学 桌面画笔悬浮窗（包括PPT控件）
			{
				WindowSearchStruct ws;
				ws.interceptType = InterceptTypeEnum::Move;
				{
					ws.windowTitle.enable = true;
					ws.windowTitle.windowTitle = LR"(工具栏)";
				}
				{
					ws.className.enable = true;
					ws.className.className = LR"(toolview)";
				}
				{
					ws.style.enable = true;
					ws.style.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
					ws.style.matchType = StyleMatchTypeEnum::Subset;
				}
				{
					ws.processName.enable = true;
					ws.processName.processName = L"teach.exe";
				}
				{
					ws.autoRecover.enable = true;
					ws.autoRecover.detectTarget = DetectObjectEnum::Iclass30Whiteboard;
				}
				windowUnionList[InterceptObjectEnum::Iclass30Floating].windows.emplace_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.interceptType = InterceptTypeEnum::Move;
				{
					ws.windowTitle.enable = true;
					ws.windowTitle.windowTitle = LR"(draw_CTransDrawWnd)";
				}
				{
					ws.className.enable = true;
					ws.className.className = LR"(draw_CTransDrawWnd)";
				}
				{
					ws.style.enable = true;
					ws.style.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_GROUP | WS_TABSTOP | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
					ws.style.matchType = StyleMatchTypeEnum::Subset;
				}
				{
					ws.processName.enable = true;
					ws.processName.processName = L"teach.exe";
				}
				{
					ws.size.enable = true;
					ws.size.MatchType = SizeMatchTypeEnum::FullScreen;
				}
				{
					ws.autoRecover.enable = true;
					ws.autoRecover.detectTarget = DetectObjectEnum::Iclass30Whiteboard;
				}
				ws.interceptScope = InterceptScopeEnum::Self;
				windowUnionList[InterceptObjectEnum::Iclass30Floating].windows.emplace_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.interceptType = InterceptTypeEnum::Move;
				{
					ws.windowTitle.enable = true;
					ws.windowTitle.windowTitle = LR"(左翻页)";
				}
				{
					ws.className.enable = true;
					ws.className.className = LR"(LeftPageCtrlView)";
				}
				{
					ws.style.enable = true;
					ws.style.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
					ws.style.matchType = StyleMatchTypeEnum::Subset;
				}
				{
					ws.processName.enable = true;
					ws.processName.processName = L"teach.exe";
				}
				{
					ws.autoRecover.enable = true;
					ws.autoRecover.detectTarget = DetectObjectEnum::Iclass30Whiteboard;
				}
				windowUnionList[InterceptObjectEnum::Iclass30Floating].windows.emplace_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.interceptType = InterceptTypeEnum::Move;
				{
					ws.windowTitle.enable = true;
					ws.windowTitle.windowTitle = LR"(右翻页)";
				}
				{
					ws.className.enable = true;
					ws.className.className = LR"(CPageCtrlView)";
				}
				{
					ws.style.enable = true;
					ws.style.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
					ws.style.matchType = StyleMatchTypeEnum::Subset;
				}
				{
					ws.processName.enable = true;
					ws.processName.processName = L"teach.exe";
				}
				{
					ws.autoRecover.enable = true;
					ws.autoRecover.detectTarget = DetectObjectEnum::Iclass30Whiteboard;
				}
				windowUnionList[InterceptObjectEnum::Iclass30Floating].windows.emplace_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.interceptType = InterceptTypeEnum::Move;
				{
					ws.windowTitle.enable = true;
					ws.windowTitle.windowTitle = LR"(PPT缩略图)";
				}
				{
					ws.className.enable = true;
					ws.className.className = LR"(ppt_CPPTThumbnailWnd)";
				}
				{
					ws.style.enable = true;
					ws.style.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
					ws.style.matchType = StyleMatchTypeEnum::Subset;
				}
				{
					ws.processName.enable = true;
					ws.processName.processName = L"teach.exe";
				}
				{
					ws.autoRecover.enable = true;
					ws.autoRecover.detectTarget = DetectObjectEnum::Iclass30Whiteboard;
				}
				windowUnionList[InterceptObjectEnum::Iclass30Floating].windows.emplace_back(ws);
			}
		}
		// Iclass30SidebarFloating
		{
			// C30智能教学 侧栏悬浮窗
			{
				WindowSearchStruct ws;
				ws.interceptType = InterceptTypeEnum::Move;
				{
					ws.windowTitle.enable = true;
					ws.windowTitle.windowTitle = LR"(控制工具条)";
				}
				{
					ws.className.enable = true;
					ws.className.className = LR"(ctrltool)";
				}
				{
					ws.style.enable = true;
					ws.style.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
					ws.style.matchType = StyleMatchTypeEnum::Subset;
				}
				{
					ws.processName.enable = true;
					ws.processName.processName = L"teach.exe";
				}
				windowUnionList[InterceptObjectEnum::Iclass30SidebarFloating].windows.emplace_back(ws);
			}
			{
				WindowSearchStruct ws;
				ws.interceptType = InterceptTypeEnum::Move;
				{
					ws.windowTitle.enable = true;
					ws.windowTitle.windowTitle = LR"(浮动工具条)";
				}
				{
					ws.className.enable = true;
					ws.className.className = LR"(fold_ctrl)";
				}
				{
					ws.style.enable = true;
					ws.style.style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
					ws.style.matchType = StyleMatchTypeEnum::Subset;
				}
				{
					ws.processName.enable = true;
					ws.processName.processName = L"teach.exe";
				}
				windowUnionList[InterceptObjectEnum::Iclass30SidebarFloating].windows.emplace_back(ws);
			}
		}
	}

	// 检测配置初始化
	{
		// 畅言4.0 白板
		{
			WindowSearchStruct ws;
			ws.interceptType = InterceptTypeEnum::Detection;
			{
				ws.windowTitle.enable = true;
				ws.windowTitle.windowTitle = LR"(畅言智慧课堂-白板[-A-Za-z]*)";
			}
			{
				ws.className.enable = true;
				ws.className.className = LR"(Qt5QWindowIcon)";
			}
			{
				ws.style.enable = true;
				ws.style.style = WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_GROUP | WS_TABSTOP | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
				ws.style.matchType = StyleMatchTypeEnum::Subset;
			}
			{
				ws.processName.enable = true;
				ws.processName.processName = L"ifly.qzk.slabBoard.exe";
			}
			detectObjectList.emplace_back(make_pair(ws, DetectObjectEnum::ChangYan4Whiteboard));
		}
		// 畅言5.0 白板
		{
			WindowSearchStruct ws;
			ws.interceptType = InterceptTypeEnum::Detection;
			{
				ws.windowTitle.enable = true;
				ws.windowTitle.windowTitle = LR"(畅言智慧课堂-白板[-A-Za-z]*)";
			}
			{
				ws.className.enable = true;
				ws.className.className = LR"(Qt5152QWindowIcon)";
			}
			{
				ws.style.enable = true;
				ws.style.style = WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_GROUP | WS_TABSTOP | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
				ws.style.matchType = StyleMatchTypeEnum::Subset;
			}
			{
				ws.processName.enable = true;
				ws.processName.processName = L"ifly.qzk.slabBoard.exe";
			}
			detectObjectList.emplace_back(make_pair(ws, DetectObjectEnum::ChangYan5Whiteboard));
		}
		// C30 白板
		{
			WindowSearchStruct ws;
			ws.interceptType = InterceptTypeEnum::Detection;
			{
				ws.windowTitle.enable = true;
				ws.windowTitle.windowTitle = LR"(超级白板\d+)";
			}
			{
				ws.className.enable = true;
				ws.className.className = LR"(wb_CWhiteBoardPageWnd)";
			}
			{
				ws.style.enable = true;
				ws.style.style = WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_GROUP | WS_TABSTOP | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
				ws.style.matchType = StyleMatchTypeEnum::Subset;
			}
			{
				ws.processName.enable = true;
				ws.processName.processName = L"super_wb.exe";
			}
			{
				ws.size.enable = true;
				ws.size.MatchType = SizeMatchTypeEnum::FullScreen;
			}
			detectObjectList.emplace_back(make_pair(ws, DetectObjectEnum::Iclass30Whiteboard));
		}
	}

	/*

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

		// 一些项缺省关闭
		{
			windowUnionList[InterceptObjectEnum::SeewoPincoSideBarFloating].enable = false;
			windowUnionList[InterceptObjectEnum::Iclass30SidebarFloating].enable = false;
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
	// 创建测试控制台
	{
#ifndef DDB_RELEASE
		{
			AllocConsole();

			FILE* fp;
			freopen_s(&fp, "CONOUT$", "w", stdout);
			freopen_s(&fp, "CONOUT$", "w", stderr);
			freopen_s(&fp, "CONIN$", "r", stdin);

			// 让 C++ 流重新与 C 的 FILE* 同步
			// true = 同步；不传参数的重载在 C++11 之后是被弃用的（某些编译器行为不定）
			std::ios::sync_with_stdio(true);

			// 清空原来的缓冲（保证重新绑定后生效）
			std::wcout.clear();
			std::wcin.clear();
			std::wcerr.clear();
			std::cout.clear();
			std::cin.clear();
			std::cerr.clear();

			std::wcout.imbue(std::locale("chs"));
		}
#endif
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
		else if (!res) cerr << "No intercept windows" << endl;
	}

	DdbWriteSetting(true);
	return 0;
}