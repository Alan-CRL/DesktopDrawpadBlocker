#pragma once

#include "DdbMain.h"

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
	Iclass30Floating,
	Iclass30SidebarFloating,

	/*
	SeewoDesktopAnnotationFloating,
	SeewoDesktopSideBarFloating,*/
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

	struct
	{
		bool enable = false;
		DetectObjectEnum detectTarget;
	} autoRecover;

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