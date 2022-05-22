#include "../include/consoleex.h"

typedef enum _SM_INTENT {
	SM_TERMINATE = 0x00,
	SM_READY = 0x01,
	SM_PRINT = 0x02
} SM_INTENT;

typedef enum _RM_INTENT {
	RM_READY = 0x01
} RM_INTENT;

#define RECMSG(msg, block) case msg: block break

BOOL APIENTRY SndMsgA(LPNAMEDPIPEA lpPipe, SM_INTENT nIntent, WORD wParam) {
	BYTE b[3];
	b[0] = (BYTE)nIntent;
	if (wParam) *(WORD*)&b[1] = wParam;

	return WriteNPipeA(lpPipe, b, 3);
}

BOOL APIENTRY SndMsgW(LPNAMEDPIPEW lpPipe, SM_INTENT nIntent, WORD wParam) {
	BYTE b[3];
	b[0] = (BYTE)nIntent;
	if (wParam) *(WORD*)&b[1] = wParam;

	return WriteNPipeW(lpPipe, b, 3);
}

BOOL APIENTRY DestroyPreCEXA(LPCONSOLEEXA lpCEX) {
	LPERRORA eptr = GetErrorA();
	ERRORA e = *eptr;

	CloseNPipeA(&lpCEX->msgrec);
	CloseNPipeA(&lpCEX->msgsend);
	CloseNPipeA(&lpCEX->prntout);

	memcpy(eptr, &e, sizeof(e));

	return FALSE;
}

BOOL APIENTRY DestroyPreCEXW(LPCONSOLEEXW lpCEX) {
	LPERRORW eptr = GetErrorW();
	ERRORW e = *eptr;

	CloseNPipeW(&lpCEX->msgrec);
	CloseNPipeW(&lpCEX->msgsend);
	CloseNPipeW(&lpCEX->prntout);

	memcpy(eptr, &e, sizeof(e));

	return FALSE;
}


BOOL APIENTRY CreateConsoleExA(
	_Out_ LPCONSOLEEXA lpConsoleEx,
	_In_ LPCSTR lpName
) {
	if (!lpConsoleEx || !lpName) THROW(NULL_PARAM, FALSE);

	lstrcpynA(lpConsoleEx->szName, lpName, 64);

	if (!CreateNPipeA(&lpConsoleEx->msgrec, lpName, "msgrec", PA_INBOUND, PM_MESSAGE, 64) ||
		!CreateNPipeA(&lpConsoleEx->msgsend, lpName, "msgsend", PA_OUTBOUND, PM_MESSAGE, 64) ||
		!CreateNPipeA(&lpConsoleEx->prntout, lpName, "prntout", PA_OUTBOUND, PM_MESSAGE, 64)) return DestroyPreCEXA(lpConsoleEx);

	char szPath[MAX_PATH];
	K32GetModuleFileNameExA(GetCurrentProcess(), GetModuleHandleA(NULL), szPath, sizeof(szPath));

	for (int i = strlen(szPath); szPath[i] != '\\'; szPath[i--] = '\0');

	strcat_s(szPath, MAX_PATH, "C_ConsoleExProc.exe ");
	strcat_s(szPath, MAX_PATH, lpName);

	system(szPath);

	BYTE b[3];
	ReadNPipeA(&lpConsoleEx->msgrec, b, 3);
	if (b != RM_READY) THROW(BAD_RESPONSE, FALSE);

	return SUCCEEDED;
}

BOOL APIENTRY CreateConsoleExW(
	_Out_ LPCONSOLEEXW lpConsoleEx,
	_In_ LPCWSTR lpName
) {
	if (!lpConsoleEx || !lpName) THROW(NULL_PARAM, FALSE);

	lstrcpynW(lpConsoleEx->szName, lpName, 64);

	if (!CreateNPipeW(&lpConsoleEx->msgrec, lpName, L"msgrec", PA_INBOUND, PM_MESSAGE, 64) ||
		!CreateNPipeW(&lpConsoleEx->msgsend, lpName, L"msgsend", PA_OUTBOUND, PM_MESSAGE, 64) ||
		!CreateNPipeW(&lpConsoleEx->prntout, lpName, L"prntout", PA_OUTBOUND, PM_MESSAGE, 64)) return DestroyPreCEXW(lpConsoleEx);

	char szPath[MAX_PATH];
	K32GetModuleFileNameExA(GetCurrentProcess(), GetModuleHandleA(NULL), szPath, sizeof(szPath));

	for (int i = strlen(szPath); szPath[i] != '\\'; szPath[i--] = '\0');

	strcat_s(szPath, MAX_PATH, "C_ConsoleExProc.exe ");
	strcat_s(szPath, MAX_PATH, lpName);

	system(szPath);

	BYTE b[3];
	ReadNPipeW(&lpConsoleEx->msgrec, b, 3);
	if (b != RM_READY) THROW(BAD_RESPONSE, FALSE);

	return SUCCEEDED;
}

BOOL APIENTRY DestroyConsoleExA(
	_In_ LPCONSOLEEXA lpConsoleEx
) {

}

BOOL APIENTRY DestroyConsoleExW(
	_In_ LPCONSOLEEXW lpConsoleEx
) {

}

BOOL APIENTRY CEXPrintFormatA(
	_In_ LPCONSOLEEXA lpConsoleEx,
	_In_ _Printf_format_string_ LPCSTR lpFormat,
	_In_ ...
) {

}

BOOL APIENTRY CEXPrintFormatW(
	_In_ LPCONSOLEEXW lpConsoleEx,
	_In_ _Printf_format_string_ LPCWSTR lpFormat,
	_In_ ...
) {

}