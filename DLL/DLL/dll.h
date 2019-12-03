#pragma once
#include <Windows.h>

#ifdef SORTED_EXPORTS
#define SORTED_API __declspec(dllexport)
#else
#define SORTED_API __declspec(dllimport)
#endif

extern "C"  SORTED_API DWORD WINAPI attr_search(LPVOID lpParam);

extern "C" SORTED_API int search();