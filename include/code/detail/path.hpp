#pragma once

#include "platform.hpp"

#include <cstdarg>

#if defined(PLATFORM_WINDOWS)
#include <windows.h>
#include <shlwapi.h>
#include <psapi.h>

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Shell32.lib")

#define VSNPRINTF		 _vsnprintf
#define VSNWPRINTF		 _vsnwprintf
#define OriSpintfUnicode  wsvnprintf
#endif

#if defined(PLATFORM_LINUX)
#define VSNPRINTF		 vsnprintf
#define VSNWPRINTF		 vswprintf
#define OriSpintfUnicode swprintf
#endif

#ifdef PLATFORM_WINDOWS
#define RunArg	wchar_t ResultArg[260];    va_list vl;	va_start(vl, Arg);	VSNWPRINTF(ResultArg, 260 - 1, Arg, vl);	va_end(vl);
#else
#define RunArg  wchar_t ResultArg[260];    va_list vl;	va_start(vl, Arg);	VSNWPRINTF(ResultArg, 260 - 1, Arg, vl);	va_end(vl);
#endif

#include "string_function.hpp"

class path_t
{

public:
	path_t()
	{	
		PathExe[0] = 0x00;
		PathDir[0] = 0x00;
	}

#ifdef PLATFORM_LINUX
	void GetPathExeLinux(wchar_t *Path) {
		char buff[260] = { 0 };
		Path[0] = 0x00;

		ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff) - 1);

		if (len != -1) {
			buff[len] = 0x00;
			cast_char(Path, buff, strlen(buff));
		}
	}
#endif

	wchar_t PathExe[260];
	wchar_t PathDir[260];

	// Получаем путь нашей программы
	wchar_t *GetPathExe() {
#ifdef  PLATFORM_WINDOWS
		GetModuleFileNameW(NULL, PathExe, sizeof(PathExe));
#else
		GetPathExeLinux(PathExe);
#endif
		return PathExe;
	}

	wchar_t *GetPathExe(wchar_t const *Arg, ...) {
		RunArg;

#ifdef PLATFORM_WINDOWS
		wsprintfW(PathDir, L"%ls%ls", GetPathExe(), ResultArg);
#endif
#ifdef PLATFORM_LINUX
		swprintf(PathDir, sizeof(PathDir) / sizeof(*PathDir), L"%ls%ls", GetPathExe(), ResultArg);
#endif

		return PathDir;
	}

	wchar_t *GetPathExe(wchar_t *Str, wchar_t const *Arg, ...) {
		RunArg;

#ifdef PLATFORM_WINDOWS
		wsprintfW(PathDir, L"%ls%ls", GetPathExe(), ResultArg);
#endif
#ifdef PLATFORM_LINUX
		swprintf(PathDir, sizeof(PathDir) / sizeof(*PathDir), L"%ls%ls", GetPathExe(), ResultArg);
#endif

		wcscpy(Str, PathDir);
		return Str;
	}

	void GetPathExe(wchar_t *Str) {
		wcscpy(Str, GetPathExe());
	}

	wchar_t *GetPathDir() {

#ifdef PLATFORM_WINDOWS
		wcscpy(PathDir, GetPathExe());
		PathRemoveFileSpecW(PathDir);
#endif

#ifdef PLATFORM_LINUX
		char   PathBuffer[260] = {};
		getcwd(PathBuffer, 260);
		cast_char(PathDir, PathBuffer);
#endif
		return PathDir;
	}

	wchar_t *GetPathDir(wchar_t const *Arg, ...)  {
	   RunArg;

	   wchar_t buffer[260] = { 0 };
	   wcscpy(buffer, GetPathDir());

#ifdef PLATFORM_WINDOWS
		wsprintfW(PathDir, L"%ls%ls", buffer, ResultArg);
#endif

#ifdef PLATFORM_LINUX

		swprintf(PathDir, sizeof(PathDir) / sizeof(*PathDir), L"%ls%ls", buffer, ResultArg);

		const int len_path = len(PathDir);

		for (int i = 0; i < len_path; i++) {
			if (PathDir[i] == '\\')
				PathDir[i] = '/';
		}
#endif


		return PathDir;
	}

	void GetPathDir(wchar_t *Str) { wcscpy(Str, GetPathDir()); }
	void GetPathDir(wchar_t *Str, wchar_t const *Arg, ...) {

		RunArg;

#ifdef PLATFORM_WINDOWS
		wsprintfW(PathDir, L"%ls%ls", GetPathDir(), ResultArg);
#endif
#ifdef PLATFORM_LINUX
		swprintf(PathDir, sizeof(PathDir) / sizeof(*PathDir), L"%ls%ls", GetPathDir(), ResultArg);
#endif

		wcscpy(Str, PathDir);
	}

#ifdef PLATFORM_WINDOWS

	wchar_t* GetPathProcess(DWORD processID) {

		HMODULE hMods[1024];
		HANDLE  hProcess;
		DWORD   cbNeeded;

		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

		if (NULL == hProcess)
			return NULL; // L"";

		if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
		{
			for (uint32_t i = 0; i < 1; i++)
				GetModuleFileNameExW(hProcess, hMods[i], PathExe, sizeof(PathExe));

			return PathExe;
		}

		CloseHandle(hProcess);
		return NULL; // L"";
	}

#endif

};

class dir_t
{
public:
	dir_t(wchar_t const *Arg, ...) {
		path_t Path;
		RunArg;
		wchar_t Dir[256];
		Path.GetPathDir(Dir);

		wcscpy(BufferPath, Dir);
		wcscat(BufferPath, ResultArg);
	}

	inline operator const wchar_t*() const { return BufferPath; }
	inline operator  wchar_t*()  { return BufferPath; }
	inline const wchar_t *Get() const { return this->BufferPath; }
	inline wchar_t *Get() { return this->BufferPath; }

	wchar_t BufferPath[256];
};