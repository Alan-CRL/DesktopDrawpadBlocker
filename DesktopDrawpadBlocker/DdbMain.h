#pragma once

#include <windows.h>
#include <iostream>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

#include <wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")

using namespace std;