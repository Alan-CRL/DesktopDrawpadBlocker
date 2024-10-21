#include "IdtText.h"

wstring utf8ToUtf16(const string& input)
{
	wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
	return converter.from_bytes(input);
}
string utf16ToUtf8(const wstring& input)
{
	wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
	return converter.to_bytes(input);
}