#ifndef LIB_MISC_H
#define LIB_MISC_H

#include <stdio.h>

#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>

#include "EnvVar.h"
#include "Const.h"
#include "Error.h"

LIB_BEGIN

bool GetSystemDateTime(
	_Out_writes_all_(_Size) char* _Buffer,
	_In_ size_t _Size,
	_In_ const char* _Format = "%h:%m:%s"
);

enum class PathRelativity { ToTarget, ToCaller, Absolute };

enum class Architecture { X86, X64 };

bool InjectDll(
	_In_ const wchar_t* _TargetProcess,
	_In_ const char* _LibraryFile,
	_In_ PathRelativity _PathRelativity = PathRelativity::ToCaller,
	_In_ Architecture _Architecture = Architecture::X64,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

LIB_END

#endif