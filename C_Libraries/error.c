#include "error.h"

LPEVENTA lpEventA = NULL;
LPEVENTW lpEventW = NULL;

VOID NewEventA(LPEVENTA lpEvent) {
	memset(lpEvent->lpfnszHandler, 0x00, sizeof(lpEvent->lpfnszHandler));
	lpEvent->nHndCount = NULL;
}

VOID NewEventW(LPEVENTW lpEvent) {
	memset(lpEvent->lpfnszHandler, 0x00, sizeof(lpEvent->lpfnszHandler));
	lpEvent->nHndCount = NULL;
}

VOID EventInvokeA(LPEVENTA lpEvent, DWORD dwCode, LPCSTR lpMessage, DWORD dwWinCode, LPCSTR lpWinMessage, LPCSTR lpFnName, DWORD dwLine, LPCSTR lpFile) {
	for (INT i = 0; i < MAX_HND_COUNT; i++)
		if (lpEventA->lpfnszHandler[i])
			lpEventA->lpfnszHandler[i](dwCode, lpMessage, dwWinCode, lpWinMessage, lpFnName, dwLine, lpFile);
}

VOID EventInvokeW(LPEVENTW lpEvent, DWORD dwCode, LPCWSTR lpMessage, DWORD dwWinCode, LPCWSTR lpWinMessage, LPCWSTR lpFnName, DWORD dwLine, LPCWSTR lpFile) {
	for (INT i = 0; i < MAX_HND_COUNT; i++)
		if (lpEventW->lpfnszHandler[i])
			lpEventW->lpfnszHandler[i](dwCode, lpMessage, dwWinCode, lpWinMessage, lpFnName, dwLine, lpFile);
}

BOOL ReturnErrorA(BOOL bReturn, LPERRORA lpError, DWORD dwCode, LPCSTR lpMessage, BOOL bWin, LPCSTR lpFnName, DWORD dwLine, LPCSTR lpFile, BOOL bInvokeHandler) {
	CHAR szWinMessage[MSGBUF_SIZE] = { '\0' };
	DWORD dwWinCode = NULL;

	if (bWin) {
		dwWinCode = GetLastError();
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwWinCode, LANG_SYSTEM_DEFAULT, szWinMessage, MSGBUF_SIZE, NULL);
	}

	if (lpEventA && bInvokeHandler) EventInvokeA(lpEventA, dwCode, lpMessage, dwWinCode, szWinMessage, lpFnName, dwLine, lpFile);
	
	if (lpError) {
		lpError->dwCode = dwCode;
		lpError->dwWinCode = dwWinCode;
		lpError->dwLine = dwLine;
		lpError->bHandlerInvoked = lpEventA && bInvokeHandler ? TRUE : FALSE;
		lstrcpyA(lpError->szMessage, lpMessage);
		lstrcpyA(lpError->szWinMessage, szWinMessage);
		lstrcpyA(lpError->szFunction, lpFnName);
		lstrcpyA(lpError->szFile, lpFile);
	}

	return bReturn;
}

BOOL ReturnErrorW(BOOL bReturn, LPERRORW lpError, DWORD dwCode, LPCWSTR lpMessage, BOOL bWin, LPCWSTR lpFnName, DWORD dwLine, LPCWSTR lpFile, BOOL bInvokeHandler) {
	WCHAR szWinMessage[MSGBUF_SIZE] = { L'\0' };
	DWORD dwWinCode = NULL;

	if (bWin) {
		dwWinCode = GetLastError();
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwWinCode, LANG_SYSTEM_DEFAULT, szWinMessage, MSGBUF_SIZE, NULL);
	}

	if (lpEventW && bInvokeHandler) EventInvokeW(lpEventW, dwCode, lpMessage, dwWinCode, szWinMessage, lpFnName, dwLine, lpFile);

	if (lpError) {
		lpError->dwCode = dwCode;
		lpError->dwWinCode = dwWinCode;
		lpError->dwLine = dwLine;
		lpError->bHandlerInvoked = lpEventW && bInvokeHandler ? TRUE : FALSE;
		lstrcpyW(lpError->szMessage, lpMessage);
		lstrcpyW(lpError->szWinMessage, szWinMessage);
		lstrcpyW(lpError->szFunction, lpFnName);
		lstrcpyW(lpError->szFile, lpFile);
	}

	return bReturn;
}

VOID SetErrorHandlerEventA(LPEVENTA lpEvent) { lpEventA = lpEvent; }

VOID SetErrorHandlerEventW(LPEVENTW lpEvent) { lpEventW = lpEvent; }

BOOL AddErrorHandlerA(LPEVENTA lpEvent, LPFNHANDLERA lpFn) {
	if (lpEvent->nHndCount >= MAX_HND_COUNT) return FALSE;
	for (INT i = 0; i < MAX_HND_COUNT; i++) if (lpEvent->lpfnszHandler[i] == lpFn) return FALSE;

	BOOL bSet = FALSE;
	for (INT i = 0; i < MAX_HND_COUNT; i++)
		if (!lpEvent->lpfnszHandler[i]) {
			lpEvent->lpfnszHandler[i] = lpFn;
			lpEvent->nHndCount++;
			bSet = TRUE;
			break;
		}

	return bSet;
}

BOOL AddErrorHandlerW(LPEVENTW lpEvent, LPFNHANDLERW lpFn) {
	if (lpEvent->nHndCount >= MAX_HND_COUNT) return FALSE;
	for (INT i = 0; i < MAX_HND_COUNT; i++) if (lpEvent->lpfnszHandler[i] == lpFn) return FALSE;

	BOOL bSet = FALSE;
	for (INT i = 0; i < MAX_HND_COUNT; i++)
		if (!lpEvent->lpfnszHandler[i]) {
			lpEvent->lpfnszHandler[i] = lpFn;
			lpEvent->nHndCount++;
			bSet = TRUE;
			break;
		}

	return bSet;
}

BOOL RemoveErrorHandlerA(LPEVENTA lpEvent, LPFNHANDLERA lpFn) {
	BOOL bFound = FALSE;
	for (INT i = 0; i < MAX_HND_COUNT; i++)
		if (lpEvent->lpfnszHandler[i] == lpFn) {
			lpEvent->lpfnszHandler[i] = NULL;
			lpEvent->nHndCount--;
			bFound = TRUE;
			break;
		}

	return bFound;
}

BOOL RemoveErrorHandlerW(LPEVENTW lpEvent, LPFNHANDLERW lpFn) {
	BOOL bFound = FALSE;
	for (INT i = 0; i < MAX_HND_COUNT; i++)
		if (lpEvent->lpfnszHandler[i] == lpFn) {
			lpEvent->lpfnszHandler[i] = NULL;
			lpEvent->nHndCount--;
			bFound = TRUE;
			break;
		}

	return bFound;
}