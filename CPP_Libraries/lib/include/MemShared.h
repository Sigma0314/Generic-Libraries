#ifndef LIB_MEMSHARED_H
#define LIB_MEMSHARED_H

#include <stdio.h>
#include <Windows.h>

#include "EnvVar.h"
#include "Const.h"
#include "Error.h"

#define MAX_PATTERN_LENGTH				256
#define CE_MIN_ADDR						0
#define CE_MAX_ADDR						0x7fffffffffff

#define HI_NIBBLE(b) (((b) >> 4) & 0x0F)
#define LO_NIBBLE(b) ((b) & 0x0F)

LIB_BEGIN

#ifdef _WIN64
typedef unsigned __int64 Address;
#else
typedef unsigned __int32 Address;
#endif

#define NEGATIVE(n) ((Address)0 - n)

typedef ArrayParam<Address> Offsets;
typedef unsigned __int8 Byte;

enum class AllocationType {
	Commit = MEM_COMMIT,
	Reserve = MEM_RESERVE,
	WriteWatch = MEM_WRITE_WATCH,
	TopDown = MEM_TOP_DOWN,
	LargePages = MEM_LARGE_PAGES,
	Physical = MEM_PHYSICAL,
	Reset = MEM_RESET,
	UndoReset = MEM_RESET_UNDO,

	Commit_Reserve = MEM_COMMIT | MEM_RESERVE,
	Commit_Reserve_TopDown = MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN,
};

enum class FreeType {
	Decommit = MEM_DECOMMIT,
	Release = MEM_RELEASE,
	Release_Coalesce = MEM_RELEASE | MEM_COALESCE_PLACEHOLDERS,
	Release_Preserve = MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER
};

enum class MemoryProtection {
	Execute_ReadWrite = PAGE_EXECUTE_READWRITE,
	Execute_Readonly = PAGE_EXECUTE_READ,
	ReadWrite = PAGE_READWRITE,
	Readonly = PAGE_READONLY
};

enum class ScanProtection {
	Exectute_ReadWrite,
	Execute_Readonly,
	Execute_Read,
	NoExecute_ReadWrite,
	NoExecute_Readonly,
	NoExecute_Read,
	ReadWrite,
	Readonly,
	Read
};

inline AllocationType operator|(AllocationType _Left, AllocationType _Right) { return (AllocationType)((DWORD)_Left | (DWORD)_Right); }
inline AllocationType operator|(AllocationType _Left, DWORD _Right) { return (AllocationType)((DWORD)_Left | (DWORD)_Right); }
inline AllocationType operator|(DWORD _Left, AllocationType _Right) { return (AllocationType)((DWORD)_Left | (DWORD)_Right); }

bool CheckVecMask(_In_ const char* _Mask, _In_ size_t _Count);

bool ParsePattern(
	_In_ const char* _ComboPattern,
	_Out_writes_all_(_PatternSize) char* _Pattern,
	_In_ size_t _PatternSize,
	_Out_writes_all_(_MaskSize) char* _Mask,
	_In_ size_t _MaskSize,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
);

bool IsProtectionValid(_In_ ScanProtection _Protection);

bool ResolveProtection(_In_ ScanProtection _DesiredProtection, _In_ DWORD _CurrentProtection);

LIB_END

#endif