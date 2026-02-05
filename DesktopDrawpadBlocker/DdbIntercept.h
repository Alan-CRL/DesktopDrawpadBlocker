#pragma once

#include "DdbMain.h"

#undef max
#undef min
#include "libcuckoo/cuckoohash_map.hh"

// 拦截类型
enum class InterceptTypeEnum
{
	Detection,
	Close,
	Hide,
	Minimize,
	Move,
};
// 拦截范围
enum class InterceptScopeEnum
{
	Self,
	Child,
	SelfAndChild,
};
// 样式匹配类型
enum class StyleMatchTypeEnum
{
	Exact,      // 精确匹配
	Subset,		// 子集匹配
};
// 尺寸匹配类型
enum class SizeMatchTypeEnum
{
	Exact,      // 精确匹配
	DPIScale,	// 跟随DPI比例
	Scale,		// 成比例
	FullScreen,	// 全屏幕
};
// 检测对象
enum class DetectObjectEnum
{
	ChangYan4Whiteboard,
	ChangYan5Whiteboard,
	Iclass30Whiteboard,
};

// 窗口追踪
extern libcuckoo::cuckoohash_map<HWND, InterceptTypeEnum> windowTracker;
class WindowTrackerRegister
{
public:
	void Register(HWND hwnd, InterceptTypeEnum interceptType)
	{
		windowTracker.insert_or_assign(hwnd, interceptType);
	}
};
void WindowTrackerStart();

// 拦截窗口
enum class InterceptObjectEnum : int
{
	SeewoWhiteboard3Floating,
	SeewoWhiteboard5Floating,
	SeewoWhiteboard5CFloating,
	SeewoPincoSideBarFloating,
	SeewoPincoDrawingFloating,
	SeewoPPTFloating,
	SeewoIwbAssistantFloating,
	YiouBoardFloating,
	AiClassFloating,
	ClassInXFloating,
	IntelligentClassFloating,
	ChangYanFloating,
	ChangYan5Floating,
	Iclass30SidebarFloating,
	Iclass30Floating,
	SeewoDesktopSideBarFloating,
	SeewoDesktopAnnotationFloating,
};
// 拦截窗口特征
struct WindowSearchStruct
{
	struct
	{
		bool enable = false;
		wstring windowTitle;
	} windowTitle;

	struct
	{
		bool enable = false;
		wstring className;
	} className;

	struct
	{
		bool enable = false;
		LONG style;
		StyleMatchTypeEnum matchType = StyleMatchTypeEnum::Subset;
	} style;

	struct
	{
		bool enable = false;
		wstring processName;
	} processName;

	struct
	{
		bool enable = false;
		int width, height;
		SizeMatchTypeEnum MatchType = SizeMatchTypeEnum::Exact;
	} size;

	// 自动恢复仅适用于 Hide 和 Move 模式
	struct
	{
		bool enable = false;
		DetectObjectEnum detectTarget;
	} autoRecover;
	// 窗口追踪仅适用于 Minimize 和 Hide 和 Move 模式
	// 注册窗口追踪后，在窗口发出关联变化的时候自动执行 DdbIntercept()
	struct
	{
		bool enable = false;
		WindowTrackerRegister windowTrackerRegister;
	} windowTracker;

	InterceptScopeEnum interceptScope = InterceptScopeEnum::SelfAndChild;
	InterceptTypeEnum interceptType = InterceptTypeEnum::Close;
};
// 拦截窗口集合
class WindowUnionClass
{
public:
	IdtAtomic<bool> enable = true;
	vector<WindowSearchStruct> windows;
};
// 拦截窗口列表
extern unordered_map<InterceptObjectEnum, WindowUnionClass> windowUnionList;

// 检测对象列表
extern vector<pair<WindowSearchStruct, DetectObjectEnum>> detectObjectList;
// 检测对象找到
extern unordered_map<DetectObjectEnum, bool> foundDetectWindows;

bool DdbIntercept();