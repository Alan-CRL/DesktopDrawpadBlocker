#pragma once

#include <windows.h>
#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

#include <wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")

using namespace std;

extern wstring buildTime;
extern string editionDate;

extern wstring userid;
extern string globalPath;
extern shared_ptr<spdlog::logger> DDBLogger;

extern bool closeSign;