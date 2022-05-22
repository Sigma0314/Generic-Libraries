#ifndef __CONSOLEEX_H__
#define __CONSOLEEX_H__

#include "envvar.h"
#include "def.h"
#include "pipes.h"

typedef struct _CONSOLEEXA {
	CHAR szName[64];
	NAMEDPIPEA msgsend;
	NAMEDPIPEA msgrec;
	NAMEDPIPEA prntout;
} CONSOLEEXA, * LPCONSOLEEXA;

typedef struct _CONSOLEEXW {
	WCHAR szName[64];
	NAMEDPIPEW msgsend;
	NAMEDPIPEW msgrec;
	NAMEDPIPEW prntout;
} CONSOLEEXW, * LPCONSOLEEXW;

BOOL APIENTRY CreateConsoleExA(
	_Out_ LPCONSOLEEXA lpConsoleEx,
	_In_ LPCSTR lpName
);

BOOL APIENTRY CreateConsoleExW(
	_Out_ LPCONSOLEEXW lpConsoleEx,
	_In_ LPCWSTR lpName
);

BOOL APIENTRY DestroyConsoleExA(
	_In_ LPCONSOLEEXA lpConsoleEx
);

BOOL APIENTRY DestroyConsoleExW(
	_In_ LPCONSOLEEXW lpConsoleEx
);

BOOL APIENTRY CEXPrintFormatA(
	_In_ LPCONSOLEEXA lpConsoleEx,
	_In_ _Printf_format_string_ LPCSTR lpFormat,
	_In_ ...
);

BOOL APIENTRY CEXPrintFormatW(
	_In_ LPCONSOLEEXW lpConsoleEx,
	_In_ _Printf_format_string_ LPCWSTR lpFormat,
	_In_ ...
);

#ifdef UNICODE

typedef CONSOLEEXW CONSOLEEX;
typedef LPCONSOLEEXW LPCONSOLEEX;

#define CreateConsoleEx CreateConsoleExW
#define DestroyConsoleEx DestroyConsoleExW
#define CEXPrintFormat CEXPrintFormatW

#else

typedef CONSOLEEXA CONSOLEEX;
typedef LPCONSOLEEXA LPCONSOLEEX;

#define CreateConsoleEx CreateConsoleExA
#define DestroyConsoleEx DestroyConsoleExA
#define CEXPrintFormat CEXPrintFormatA

#endif // UNICODE

#endif // !__CONSOLEEX_H__