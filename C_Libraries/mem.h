#ifndef __MEM_H__
#define __MEM_H__

#include "def.h"

#include <Windows.h>
#include <Psapi.h>

#define Read(ty, addr) *(ty*)(addr)
#define Write(ty, addr, val) *(ty*)(addr) = val

typedef struct _PROCNFOA {
	HANDLE hProcess;
	HMODULE hModule;
	CHAR szProcName[MAX_PATH];
	DWORD dwProcId;
	ADDRESS uModBase;
	DWORD dwModSize;
} PROCNFOA, * LPPROCNFOA;

typedef struct _PROCNFOW {
	HANDLE hProcess;
	HMODULE hModule;
	WCHAR szProcName[MAX_PATH];
	DWORD dwProcId;
	ADDRESS uModBase;
	DWORD dwModSize;
} PROCNFOW, * LPPROCNFOW;

typedef struct _MODNFOA {
	HMODULE hModule;
	CHAR szModName[MAX_PATH];
	ADDRESS uModBase;
	DWORD dwModSize;
} MODNFOA, * LPMODNFOA;

typedef struct _MODNFOW {
	HMODULE hModule;
	WCHAR szModName[MAX_PATH];
	ADDRESS uModBase;
	DWORD dwModSize;
} MODNFOW, * LPMODNFOW;

BOOL GetProcessInfoA(LPPROCNFOA lppnfo);
BOOL GetProcessInfoW(LPPROCNFOW lppnfo);

BOOL GetModuleInfoA(LPMODNFOA lpmnfo, LPCSTR lpModName);
BOOL GetModuleInfoW(LPMODNFOW lpmnfo, LPCWSTR lpModName);

#ifdef UNICODE
typedef PROCNFOW PROCNFO, * LPPROCNFO;
typedef MODNFOW MODNFO, * LPMODNFO;

#define GetProcessInfo GetProcessInfoW
#define GetModuleInfo GetModuleInfoW
#else
typedef PROCNFOA PROCNFO, * LPPROCNFO;
typedef MODNFOA MODNFO, * LPMODNFO;

#define GetProcessInfo GetProcessInfoA
#define GetModuleInfo GetModuleInfoA
#endif // UNICODE

BOOL CheckPtr(ADDRESS uAddress, DWORD dwSize);

#endif // !__MEM_H__