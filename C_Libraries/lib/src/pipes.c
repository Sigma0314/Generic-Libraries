#include "../include/pipes.h"

BOOL APIENTRY CreateAPipe(
	_Out_ LPPIPE lpPipe,
	_In_ DWORD dwSize
) {
	if (!lpPipe) THROW(NULL_PARAM, FALSE);

	if (!CreatePipe(&lpPipe->hReadPipe, &lpPipe->hWritePipe, NULL, dwSize)) THROW(WIN_FUNCTION, TRUE);

	lpPipe->dwSize = dwSize;

	lpPipe->bActive = TRUE;
	return SUCCEEDED;
}

BOOL APIENTRY CreateNPipeA(
	_Out_ LPNAMEDPIPEA lpPipe,
	_In_ LPCSTR lpBaseName,
	_In_opt_ LPCSTR lpNameSuffix,
	_In_ PIPE_ACCESS nPipeAccess,
	_In_ PIPE_MODE nPipeMode,
	_In_ DWORD dwSize
) {
	if (!lpPipe || !lpBaseName) THROW(NULL_PARAM, FALSE);

	sprintf_s(lpPipe->szName, MAX_PATH, "\\\\.\\pipe\\%s%s", lpBaseName, lpNameSuffix ? lpNameSuffix : "");

	lpPipe->hPipe = CreateNamedPipeA(lpPipe->szName, (DWORD)nPipeAccess, (DWORD)nPipeMode, PIPE_UNLIMITED_INSTANCES, dwSize, dwSize, INFINITE, NULL);
	if (lpPipe->hPipe == INVALID_HANDLE_VALUE) THROW(WIN_FUNCTION, TRUE);

	lpPipe->dwSize = dwSize;
	lpPipe->nPipeAccess = nPipeAccess;

	lpPipe->bActive = TRUE;
	return SUCCEEDED;
}

BOOL APIENTRY CreateNPipeW(
	_Out_ LPNAMEDPIPEW lpPipe,
	_In_ LPCWSTR lpBaseName,
	_In_opt_ LPCWSTR lpNameSuffix,
	_In_ PIPE_ACCESS nPipeAccess,
	_In_ PIPE_MODE nPipeMode,
	_In_ DWORD dwSize
) {
	if (!lpPipe || !lpBaseName) THROW(NULL_PARAM, FALSE);

	swprintf_s(lpPipe->szName, MAX_PATH, L"\\\\.\\pipe\\%s%s", lpBaseName, lpNameSuffix ? lpNameSuffix : L"");

	lpPipe->hPipe = CreateNamedPipeW(lpPipe->szName, (DWORD)nPipeAccess, (DWORD)nPipeMode, PIPE_UNLIMITED_INSTANCES, dwSize, dwSize, INFINITE, NULL);
	if (lpPipe->hPipe == INVALID_HANDLE_VALUE) THROW(WIN_FUNCTION, TRUE);

	lpPipe->dwSize = dwSize;
	lpPipe->nPipeAccess = nPipeAccess;

	lpPipe->bActive = TRUE;
	return SUCCEEDED;
}

BOOL APIENTRY CloseAPipe(
	_In_ LPPIPE lpPipe
) {
	if (!lpPipe) THROW(NULL_PARAM, FALSE);

	if (CloseHandle(lpPipe->hReadPipe) && CloseHandle(lpPipe->hWritePipe)) {
		lpPipe->bActive = FALSE;
		return SUCCEEDED;
	}

	else THROW(WIN_FUNCTION, TRUE);
}

BOOL APIENTRY CloseNPipeA(
	_In_ LPNAMEDPIPEA lpPipe
) {
	if (!lpPipe) THROW(NULL_PARAM, FALSE);

	if (!CloseHandle(lpPipe->hPipe)) THROW(WIN_FUNCTION, TRUE);
	
	lpPipe->bActive = FALSE;
	return SUCCEEDED;
}

BOOL APIENTRY CloseNPipeW(
	_In_ LPNAMEDPIPEW lpPipe
) {
	if (!lpPipe) THROW(NULL_PARAM, FALSE);

	if (!CloseHandle(lpPipe->hPipe)) THROW(WIN_FUNCTION, TRUE);

	lpPipe->bActive = FALSE;
	return SUCCEEDED;
}

BOOL APIENTRY OpenNPipeA(
	_Out_ LPNAMEDPIPEA lpPipe,
	_In_ LPCSTR lpBaseName,
	_In_opt_ LPCSTR lpNameSuffix,
	_In_ PIPE_ACCESS nPipeAccess
) {
	if (!lpPipe) THROW(NULL_PARAM, FALSE);

	sprintf_s(lpPipe->szName, MAX_PATH, "\\\\.\\pipe\\%s%s", lpBaseName, lpNameSuffix ? lpNameSuffix : "");

	lpPipe->hPipe = CreateFileA(
		lpPipe->szName,
		nPipeAccess == PA_INBOUND ? GENERIC_READ : (nPipeAccess == PA_OUTBOUND ? GENERIC_WRITE : GENERIC_READ | GENERIC_WRITE),
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (lpPipe->hPipe == INVALID_HANDLE_VALUE) THROW(WIN_FUNCTION, TRUE);

	lpPipe->dwSize = -1;
	lpPipe->nPipeAccess = nPipeAccess;

	lpPipe->bActive = TRUE;
	return SUCCEEDED;
}

BOOL APIENTRY OpenNPipeW(
	_Out_ LPNAMEDPIPEW lpPipe,
	_In_ LPCWSTR lpBaseName,
	_In_opt_ LPCWSTR lpNameSuffix,
	_In_ PIPE_ACCESS nPipeAccess
) {
	if (!lpPipe) THROW(NULL_PARAM, FALSE);

	swprintf_s(lpPipe->szName, MAX_PATH, L"\\\\.\\pipe\\%s%s", lpBaseName, lpNameSuffix ? lpNameSuffix : L"");

	lpPipe->hPipe = CreateFileW(
		lpPipe->szName,
		nPipeAccess == PA_INBOUND ? GENERIC_READ : (nPipeAccess == PA_OUTBOUND ? GENERIC_WRITE : GENERIC_READ | GENERIC_WRITE),
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (lpPipe->hPipe == INVALID_HANDLE_VALUE) THROW(WIN_FUNCTION, TRUE);

	lpPipe->dwSize = -1;
	lpPipe->nPipeAccess = nPipeAccess;

	lpPipe->bActive = TRUE;
	return SUCCEEDED;
}

BOOL APIENTRY ReadAPipe(
	_In_ LPPIPE lpPipe,
	_Out_writes_all_(dwSize) LPVOID lpBuffer,
	_In_ DWORD dwSize
) {
	if (!lpPipe) THROW(NULL_PARAM, FALSE);

	if (!ReadFile(lpPipe->hReadPipe, lpBuffer, dwSize, NULL, NULL)) THROW(WIN_FUNCTION, TRUE);

	return SUCCEEDED;
}

BOOL APIENTRY ReadNPipeA(
	_In_ LPNAMEDPIPEA lpPipe,
	_Out_writes_all_(dwSize) LPVOID lpBuffer,
	_In_ DWORD dwSize
) {
	if (!lpPipe) THROW(NULL_PARAM, FALSE);

	if (!ReadFile(lpPipe->hPipe, lpBuffer, dwSize, NULL, NULL)) THROW(WIN_FUNCTION, TRUE);

	return SUCCEEDED;
}

BOOL APIENTRY ReadNPipeW(
	_In_ LPNAMEDPIPEW lpPipe,
	_Out_writes_all_(dwSize) LPVOID lpBuffer,
	_In_ DWORD dwSize
) {
	if (!lpPipe) THROW(NULL_PARAM, FALSE);

	if (!ReadFile(lpPipe->hPipe, lpBuffer, dwSize, NULL, NULL)) THROW(WIN_FUNCTION, TRUE);

	return SUCCEEDED;
}

BOOL APIENTRY WriteAPipe(
	_In_ LPPIPE lpPipe,
	_In_ LPCVOID lpBuffer,
	_In_ DWORD dwSize
) {
	if (!lpPipe) THROW(NULL_PARAM, FALSE);

	if (!WriteFile(lpPipe->hWritePipe, lpBuffer, dwSize, NULL, NULL)) THROW(WIN_FUNCTION, TRUE);

	return SUCCEEDED;
}

BOOL APIENTRY WriteNPipeA(
	_In_ LPNAMEDPIPEA lpPipe,
	_In_ LPCVOID lpBuffer,
	_In_ DWORD dwSize
) {
	if (!lpPipe) THROW(NULL_PARAM, FALSE);

	if (!WriteFile(lpPipe->hPipe, lpBuffer, dwSize, NULL, NULL)) THROW(WIN_FUNCTION, TRUE);

	return SUCCEEDED;
}

BOOL APIENTRY WriteNPipeW(
	_In_ LPNAMEDPIPEW lpPipe,
	_In_ LPCVOID lpBuffer,
	_In_ DWORD dwSize
) {
	if (!lpPipe) THROW(NULL_PARAM, FALSE);

	if (!WriteFile(lpPipe->hPipe, lpBuffer, dwSize, NULL, NULL)) THROW(WIN_FUNCTION, TRUE);

	return SUCCEEDED;
}