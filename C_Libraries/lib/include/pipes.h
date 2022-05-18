#ifndef __PIPES_H__
#define __PIPES_H__

#include <Windows.h>
#include <Psapi.h>
#include <stdio.h>

#include "envvar.h"
#include "def.h"
#include "error.h"

typedef enum _PIPE_ACCESS {
	PA_INBOUND = PIPE_ACCESS_INBOUND,
	PA_OUTBOUND = PIPE_ACCESS_OUTBOUND,
	PA_DUPLEX = PIPE_ACCESS_DUPLEX
} PIPE_ACCESS, * LPPIPE_ACCESS;

typedef enum _PIPE_MODE {
	PM_MESSAGE = PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT | PIPE_ACCEPT_REMOTE_CLIENTS,
	PM_DATA = PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT | PIPE_ACCEPT_REMOTE_CLIENTS
} PIPE_MODE, * LPPIPE_MODE;

typedef struct _PIPE {
	HANDLE hReadPipe;
	HANDLE hWritePipe;
	DWORD dwSize;
	BOOL bActive;
} PIPE, * LPPIPE;

typedef struct _NAMEDPIPEA {
	CHAR szName[MAX_PATH];
	HANDLE hPipe;
	DWORD dwSize;
	PIPE_ACCESS nPipeAccess;
	BOOL bActive;
} NAMEDPIPEA, * LPNAMEDPIPEA;

typedef struct _NAMEDPIPEW {
	WCHAR szName[MAX_PATH];
	HANDLE hPipe;
	DWORD dwSize;
	PIPE_ACCESS nPipeAccess;
	BOOL bActive;
} NAMEDPIPEW, * LPNAMEDPIPEW;

BOOL APIENTRY CreateAPipe(
	_Out_ LPPIPE lpPipe,
	_In_ DWORD dwSize
);

BOOL APIENTRY CreateNPipeA(
	_Out_ LPNAMEDPIPEA lpPipe,
	_In_ LPCSTR lpBaseName,
	_In_opt_ LPCSTR lpNameSuffix,
	_In_ PIPE_ACCESS nPipeAccess,
	_In_ PIPE_MODE nPipeMode,
	_In_ DWORD dwSize
);

BOOL APIENTRY CreateNPipeW(
	_Out_ LPNAMEDPIPEW lpPipe,
	_In_ LPCWSTR lpBaseName,
	_In_opt_ LPCWSTR lpNameSuffix,
	_In_ PIPE_ACCESS nPipeAccess,
	_In_ PIPE_MODE nPipeMode,
	_In_ DWORD dwSize
);

BOOL APIENTRY CloseAPipe(
	_In_ LPPIPE lpPipe
);

BOOL APIENTRY CloseNPipeA(
	_In_ LPNAMEDPIPEA lpPipe
);

BOOL APIENTRY CloseNPipeW(
	_In_ LPNAMEDPIPEW lpPipe
);

BOOL APIENTRY OpenNPipeA(
	_Out_ LPNAMEDPIPEA lpPipe,
	_In_ LPCSTR lpBaseName,
	_In_opt_ LPCSTR lpNameSuffix,
	_In_ PIPE_ACCESS nPipeAccess
);

BOOL APIENTRY OpenNPipeW(
	_Out_ LPNAMEDPIPEW lpPipe,
	_In_ LPCWSTR lpBaseName,
	_In_opt_ LPCWSTR lpNameSuffix,
	_In_ PIPE_ACCESS nPipeAccess
);

BOOL APIENTRY ReadAPipe(
	_In_ LPPIPE lpPipe,
	_Out_writes_all_(dwSize) LPVOID lpBuffer,
	_In_ DWORD dwSize
);

BOOL APIENTRY ReadNPipeA(
	_In_ LPNAMEDPIPEA lpPipe,
	_Out_writes_all_(dwSize) LPVOID lpBuffer,
	_In_ DWORD dwSize
);

BOOL APIENTRY ReadNPipeW(
	_In_ LPNAMEDPIPEW lpPipe,
	_Out_writes_all_(dwSize) LPVOID lpBuffer,
	_In_ DWORD dwSize
);

BOOL APIENTRY WriteAPipe(
	_In_ LPPIPE lpPipe,
	_In_ LPCVOID lpBuffer,
	_In_ DWORD dwSize
);

BOOL APIENTRY WriteNPipeA(
	_In_ LPNAMEDPIPEA lpPipe,
	_In_ LPCVOID lpBuffer,
	_In_ DWORD dwSize
);

BOOL APIENTRY WriteNPipeW(
	_In_ LPNAMEDPIPEW lpPipe,
	_In_ LPCVOID lpBuffer,
	_In_ DWORD dwSize
);

#ifdef UNICODE

typedef NAMEDPIPEW NAMEDPIPE;
typedef LPNAMEDPIPEW LPNAMEDPIPE;

#define CreateNPipe CreateNPipeW
#define CloseNPipe CloseNPipeW
#define OpenNPipe OpenNPipeW
#define ReadNPipe ReadNPipeW
#define WriteNPipe WriteNPipeW

#else

typedef NAMEDPIPEA NAMEDPIPE;
typedef LPNAMEDPIPEA LPNAMEDPIPE;

#define CreateNPipe CreateNPipeA
#define CloseNPipe CloseNPipeA
#define OpenNPipe OpenNPipeA
#define ReadNPipe ReadNPipeA
#define WriteNPipe WriteNPipeA

#endif // UNICODE

#endif // !__PIPES_H__