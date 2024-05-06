#pragma once

#include "DdbMain.h"

//string to wstring
wstring StringToWstring(const string& s);
//wstring to string
string WstringToString(const wstring& ws);

//string to LPCWSTR
LPCWSTR StringToLPCWSTR(string str);

//string to urlencode
string ConvertToUrlencode(string str);
//utf-8 to GBK
string ConvertToGbk(string strUTF8);
//GBK to utf-8
string ConvertToUtf8(string str);