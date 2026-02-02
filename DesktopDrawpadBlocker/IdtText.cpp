#include "IdtText.h"

wstring utf8ToUtf16(const string& input)
{
	if (input.empty()) return wstring();
	int len = MultiByteToWideChar(CP_UTF8, 0, input.data(), (int)input.size(), nullptr, 0);
	wstring result(len, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, input.data(), (int)input.size(), &result[0], len);
	return result;
}
string utf16ToUtf8(const wstring& input)
{
	if (input.empty()) return string();
	int len = WideCharToMultiByte(CP_UTF8, 0, input.data(), (int)input.size(), nullptr, 0, nullptr, nullptr);
	string result(len, '\0');
	WideCharToMultiByte(CP_UTF8, 0, input.data(), (int)input.size(), &result[0], len, nullptr, nullptr);
	return result;
}