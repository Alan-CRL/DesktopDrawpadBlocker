#pragma once
#define DDB_RELEASE

#include <windows.h>
#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <codecvt>

#include <wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")

using namespace std;

extern wstring buildTime;
extern wstring editionDate;

extern wstring userid;
extern wstring globalPath;

#ifndef DDB_RELEASE
void Test();
void Testb(bool t);
void Testi(long long t);
void Testw(wstring t);
void Testa(string t);
#endif

extern bool closeSign;
extern int occSErrorT;