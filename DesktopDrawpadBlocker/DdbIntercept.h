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

// 拦截窗口
enum class InterceptObjectEnum
{
	Seewowhiteboard3Floating,
	Seewowhiteboard5Floating,
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
	Iclass30Floating，
	Iclass30SidebarFloating,
};
// 拦截窗口列表
unordered_map<InterceptObjectEnum, WindowUnionClass> windowUnionList;
// 拦截窗口集合
class WindowUnionClass
{
public:
	IdtAtomic<bool> enable = false;
	vector<WindowSearchStruct> windows;
};
// 拦截窗口特征
struct WindowSearchStruct
{
	bool hasClassName;
	wstring className;

	bool hasWindowTitle;
	wstring windowTitle;

	bool hasStyle;
	LONG style;
	StyleMatchTypeEnum styleMatchType = StyleMatchTypeEnum::Exact;

	bool hasWidthHeight;
	int width, height;
	SizeMatchTypeEnum sizeMatchType = SizeMatchTypeEnum::Exact;

	InterceptTypeEnum interceptType = InterceptTypeEnum::Close;
	int prevX = 0, prevY = 0; // 自动恢复位置
};

// 检测对象
enum class DetectObjectEnum
{
	Iclass30Whiteboard,
	ChangYanWhiteboard,
};
// 检测对象列表
vector<pair<WindowSearchStruct, DetectObjectEnum>> detectObjectList;
// 检测对象找到
unordered_map<DetectObjectEnum, bool> detectObjectFoundMap;

bool DdbIntercept();