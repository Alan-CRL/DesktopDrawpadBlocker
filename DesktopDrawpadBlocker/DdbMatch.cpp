#include "DdbMatch.h"

bool RegexMatch(const wstring& pattern, const wstring& text)
{
	if (pattern.empty() && text.empty()) return true;

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
bool CheckMatch(const MatchStruct& matchStruct, const wstring& text)
{
	for (const auto& part : matchStruct.parts)
	{
		if (RegexMatch(part, text))
			return true;
	}
	return false;
}