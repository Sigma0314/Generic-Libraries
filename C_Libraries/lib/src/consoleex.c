#include "../include/consoleex.h"

typedef enum _SM_INTENT {
	SM_READY = 0x01
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

VOID APIENTRY ThreadA(LPCONSOLEEXA lpConsoleEx) {
	BYTE buf[64];

	while (ReadNPipeA(lpConsoleEx, buf, 64)) {
		switch ((RM_INTENT)buf[0]) {
			RECMSG(RM_READY, { });
		}
	}
}

VOID APIENTRY ThreadW(LPCONSOLEEXW lpConsoleEx) {

}

BOOL CreateRecThreadA(LPCONSOLEEXA lpConsoleEx, LPHANDLE lphThread) { *lphThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ThreadA, lpConsoleEx, NULL, NULL); if (!*lphThread) return FALSE; return TRUE; }
BOOL CreateRecThreadW(LPCONSOLEEXW lpConsoleEx, LPHANDLE lphThread) { *lphThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ThreadW, lpConsoleEx, NULL, NULL); if (!*lphThread) return FALSE; return TRUE; }

BOOL APIENTRY CreateConsoleExA(
	_Out_ LPCONSOLEEXA lpConsoleEx,
	_In_ LPCSTR lpName
) {
	if (!lpConsoleEx || !lpName) THROW(NULLPARAM, FALSE);

	if (!CreateNPipeA(&lpConsoleEx->msgrec, lpName, "msgrec", PA_INBOUND, PM_MESSAGE, 64) ||
		!CreateNPipeA(&lpConsoleEx->msgsend, lpName, "msgsend", PA_OUTBOUND, PM_MESSAGE, 64) ||
		!CreateNPipeA(&lpConsoleEx->prntout, lpName, "prntout", PA_OUTBOUND, PM_MESSAGE, 64)) return FALSE;
}

BOOL APIENTRY CreateConsoleExW(
	_Out_ LPCONSOLEEXW lpConsoleEx,
	_In_ LPCWSTR lpName
) {

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