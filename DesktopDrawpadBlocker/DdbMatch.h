#pragma once

#include "DdbMain.h"

#include <regex>

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

bool CheckMatch(const MatchStruct& matchStruct, const wstring& text)
{
	for (const auto& part : matchStruct.parts)
	{
		if (RegexMatch(part, text))
			return true;
	}
	return false;
}
bool RegexMatch(const wstring& pattern, const wstring& text)
{
	try
	{
		wregex regexPattern(pattern);
		return regex_match(text, regexPattern);
	}
	catch (const regex_error&)
	{
		return false;
	}
}