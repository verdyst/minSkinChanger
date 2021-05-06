#pragma once
#include <Windows.h>
#include "ntdll.h"
#include <string>
#include <iostream>

uintptr_t ScanModIn(const char* pattern, const char* mask, const char* dllName, int offset, int extra);

uintptr_t ptrScanModIn(char const* pattern, char const* mask, char const* dllName, int offset, int extra);

uintptr_t altScanModIn(const char* pattern, const char* dllName, int offset, int extra);

uintptr_t altScanModIn2(const char* pattern, const char* dllName, int offset, int extra);