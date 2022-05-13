#ifndef __ERROR_H__
#define __ERROR_H__

#include <Windows.h>

#undef ERROR

#define MSGBUF_SIZE 256
#define MAX_HND_COUNT 64

#define DEBUG_PARAM_A __FUNCTION__, __LINE__, __FILE__
#define DEBUG_PARAM_W __FUNCTIONW__, __LINE__, __FILEW__

#define HND_PARAM_A DWORD dwCode, LPCSTR lpMessage, DWORD dwWinCode, LPCSTR lpWinMessage, LPCSTR lpFnName, DWORD dwLine, LPCSTR lpFile
#define HND_PARAM_W DWORD dwCode, LPCWSTR lpMessage, DWORD dwWinCode, LPCWSTR lpWinMessage, LPCWSTR lpFnName, DWORD dwLine, LPCWSTR lpFile

typedef VOID(CDECL* LPFNHANDLERA)(HND_PARAM_A);
typedef VOID(CDECL* LPFNHANDLERW)(HND_PARAM_W);

typedef struct _ERRORA {
	DWORD dwCode;
	DWORD dwWinCode;
	DWORD dwLine;
	BOOL bHandlerInvoked;
	CHAR szMessage[MSGBUF_SIZE];
	CHAR szWinMessage[MSGBUF_SIZE];
	CHAR szFunction[MSGBUF_SIZE];
	CHAR szFile[MAX_PATH];
} ERRORA, * LPERRORA;

typedef struct _ERRORW {
	DWORD dwCode;
	DWORD dwWinCode;
	DWORD dwLine;
	BOOL bHandlerInvoked;
	WCHAR szMessage[MSGBUF_SIZE];
	WCHAR szWinMessage[MSGBUF_SIZE];
	WCHAR szFunction[MSGBUF_SIZE];
	WCHAR szFile[MAX_PATH];
} ERRORW, * LPERRORW;

typedef struct _EVENTA {
	LPFNHANDLERA lpfnszHandler[MAX_HND_COUNT];
	SIZE_T nHndCount;
} EVENTA, * LPEVENTA;

typedef struct _EVENTW {
	LPFNHANDLERW lpfnszHandler[MAX_HND_COUNT];
	SIZE_T nHndCount;
} EVENTW, * LPEVENTW;

VOID NewEventA(LPEVENTA lpEvent);
VOID NewEventW(LPEVENTW lpEvent);

VOID EventInvokeA(LPEVENTA lpEvent, DWORD dwCode, LPCSTR lpMessage, DWORD dwWinCode, LPCSTR lpWinMessage, LPCSTR lpFnName, DWORD dwLine, LPCSTR lpFile);
VOID EventInvokeW(LPEVENTW lpEvent, DWORD dwCode, LPCWSTR lpMessage, DWORD dwWinCode, LPCWSTR lpWinMessage, LPCWSTR lpFnName, DWORD dwLine, LPCWSTR lpFile);

BOOL ReturnErrorA(BOOL bReturn, LPERRORA lpError, DWORD dwCode, LPCSTR lpMessage, BOOL bWin, LPCSTR lpFnName, DWORD dwLine, LPCSTR lpFile, BOOL bInvokeHandler);
BOOL ReturnErrorW(BOOL bReturn, LPERRORW lpError, DWORD dwCode, LPCWSTR lpMessage, BOOL bWin, LPCWSTR lpFnName, DWORD dwLine, LPCWSTR lpFile, BOOL bInvokeHandler);

VOID SetErrorHandlerEventA(LPEVENTA lpEvent);
VOID SetErrorHandlerEventW(LPEVENTW lpEvent);

BOOL AddErrorHandlerA(LPEVENTA lpEvent, LPFNHANDLERA lpFn);
BOOL AddErrorHandlerW(LPEVENTW lpEvent, LPFNHANDLERW lpFn);

BOOL RemoveErrorHandlerA(LPEVENTA lpEvent, LPFNHANDLERA lpFn);
BOOL RemoveErrorHandlerW(LPEVENTW lpEvent, LPFNHANDLERW lpFn);

#ifdef UNICODE
	typedef ERRORW ERROR, * LPERROR;
	typedef LPFNHANDLERW LPFNHANDLER;
	typedef EVENTW EVENT, * LPEVENT;
	
	#define ReturnError ReturnErrorW
	#define SetErrorHandlerEvent SetErrorHandlerEventW
	#define AddErrorHandler AddErrorHandlerW
	#define RemoveErrorHandler RemoveErrorHandlerW
	#define NewEvent NewEventW

	#define DEBUG_PARAM DEBUG_PARAM_W
	#define HND_PARAM HND_PARAM_W
#else
	typedef ERRORA ERROR, * LPERROR;
	typedef LPFNHANDLERA LPFNHANDLER;
	typedef EVENTA EVENT, * LPEVENT;
	
	#define ReturnError ReturnErrorA
	#define SetErrorHandlerEvent SetErrorHandlerEventA
	#define AddErrorHandler AddErrorHandlerA
	#define RemoveErrorHandler RemoveErrorHandlerA
	#define NewEvent NewEventA

	#define DEBUG_PARAM DEBUG_PARAM_A
	#define HND_PARAM HND_PARAM_A
#endif

#endif // !__ERROR_H__