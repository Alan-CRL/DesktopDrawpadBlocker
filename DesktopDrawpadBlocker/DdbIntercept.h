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
};
// 检测对象
enum class DetectObjectEnum
{
	Iclass30Whiteboard,
	ChangYanWhiteboard,
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
	AiClassFloating,
	HiteAnnotationFloating,
	ChangYanFloating,
	ChangYanPptFloating,
	IntelligentClassFloating,
	SeewoDesktopAnnotationFloating,
	SeewoDesktopSideBarFloating,
	Iclass30Floating,
	Iclass30SidebarFloating,
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
		int prevX = 0, prevY = 0; // 自动恢复位置
	} autoRecover;

	InterceptTypeEnum interceptType = InterceptTypeEnum::Close;
};
// 拦截窗口集合
class WindowUnionClass
{
public:
	IdtAtomic<bool> enable = false;
	vector<WindowSearchStruct> windows;
};
// 拦截窗口列表
extern unordered_map<InterceptObjectEnum, WindowUnionClass> windowUnionList;

// 检测对象列表
extern vector<pair<WindowSearchStruct, DetectObjectEnum>> detectObjectList;
// 检测对象找到
extern unordered_map<DetectObjectEnum, bool> detectObjectFoundMap;

bool DdbIntercept();