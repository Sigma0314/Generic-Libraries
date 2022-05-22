#include "../include/error.h"

#include <string.h>

ERRORA ea;
ERRORW ew;

#define ASSIGN_ERROR_STRINGA(nCode, lpString) case nCode: lstrcpyA(lpBuffer, lpString); return;
#define ASSIGN_ERROR_STRINGW(nCode, lpString) case nCode: lstrcpyW(lpBuffer, lpString); return;

VOID APIENTRY GetMessageFromCodeA(ERROR_CODE nCode, LPSTR lpBuffer) {
	switch (nCode) {
		ASSIGN_ERROR_STRINGA(SUCCESS, "The function has succeeded.");
		ASSIGN_ERROR_STRINGA(NULL_PARAM, "An essential parameter was NULL.");
		ASSIGN_ERROR_STRINGA(INVALID_PARAM, "An essential parameter was invalid.");
		ASSIGN_ERROR_STRINGA(ALREADY_REGISTERED, "The referenced struct is already registered.");
		ASSIGN_ERROR_STRINGA(WIN_FUNCTION, "A WinApi function has failed, see the WinApi error details.");
		ASSIGN_ERROR_STRINGA(BAD_RESPONSE, "The response from the remote process was invalid.");
	default: lstrcpyA(lpBuffer, "Unknown error code."); return;
	}
}

VOID APIENTRY GetMessageFromCodeW(ERROR_CODE nCode, LPWSTR lpBuffer) {
	switch (nCode) {
		ASSIGN_ERROR_STRINGW(SUCCESS, L"The function has succeeded.");
		ASSIGN_ERROR_STRINGW(NULL_PARAM, L"An essential parameter was NULL.");
		ASSIGN_ERROR_STRINGW(INVALID_PARAM, L"An essential parameter was invalid.");
		ASSIGN_ERROR_STRINGW(ALREADY_REGISTERED, L"The referenced struct is already registered.");
		ASSIGN_ERROR_STRINGW(WIN_FUNCTION, L"A WinApi function has failed, see the WinApi error details.");
		ASSIGN_ERROR_STRINGW(BAD_RESPONSE, L"The response from the remote process was invalid.");
	default: lstrcpyW(lpBuffer, L"Unknown error code."); return;
	}
}

BOOL APIENTRY SetErrorA(
	BOOL bReturn,
	ERROR_CODE nCode,
	BOOL bWin,
	LPCSTR lpFunction,
	DWORD dwLine,
	LPCSTR lpFile
) {
	ea.nCode = nCode;
	GetMessageFromCodeA(nCode, ea.szMessage);
	if (bWin) {
		ea.dwWinCode = GetLastError();
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, ea.dwWinCode, LANG_SYSTEM_DEFAULT, ea.szWinMessage, MSG_SIZE, NULL);
	}
	else {
		ea.dwWinCode = NULL;
		ea.szWinMessage[0] = '\0';
	}

	lstrcpyA(ea.szFunction, lpFunction);
	ea.dwLine = dwLine;
	lstrcpyA(ea.szFile, lpFile);

	return bReturn;
}

BOOL APIENTRY SetErrorW(
	BOOL bReturn,
	ERROR_CODE nCode,
	BOOL bWin,
	LPCWSTR lpFunction,
	DWORD dwLine,
	LPCWSTR lpFile
) {
	ew.nCode = nCode;
	GetMessageFromCodeW(nCode, ew.szMessage);
	if (bWin) {
		ew.dwWinCode = GetLastError();
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, ew.dwWinCode, LANG_SYSTEM_DEFAULT, ew.szWinMessage, MSG_SIZE, NULL);
	}
	else {
		ew.dwWinCode = NULL;
		ew.szWinMessage[0] = L'\0';
	}

	lstrcpyW(ew.szFunction, lpFunction);
	ew.dwLine = dwLine;
	lstrcpyW(ew.szFile, lpFile);

	return bReturn;
}

LPERRORA APIENTRY GetErrorA() { return &ea; };
LPERRORW APIENTRY GetErrorW() { return &ew; };