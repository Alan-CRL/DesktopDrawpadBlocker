#pragma once

#include "DdbMain.h"

struct MatchStruct
{
	MatchStruct(wstring str1)
	{
		parts.push_back(str1);
	}
	MatchStruct(wstring str1, wstring str2)
	{
		parts.push_back(str1);
		parts.push_back(str2);
	}

	vector<wstring> parts;
};

bool RegexMatch(const wstring& pattern, const wstring& text);
bool CheckMatch(const MatchStruct& matchStruct, const wstring& text);