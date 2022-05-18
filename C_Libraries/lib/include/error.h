#ifndef __ERROR_H__
#define __ERROR_H__

#include <Windows.h>
#undef ERROR

#include "envvar.h"
#include "def.h"

#define MSG_SIZE 256
#define FN_NAME_SIZE 64

#define EPARAMA __FUNCTION__, __LINE__, __FILE__
#define EPARAMW __FUNCTIONW__, __LINE__, __FILEW__

#define THROWA(nCode, bWin) return SetErrorA(FALSE, nCode, bWin, EPARAMA)
#define THROWW(nCode, bWin) return SetErrorW(FALSE, nCode, bWin, EPARAMW)

#define SUCCEEDEDA SetErrorA(TRUE, SUCCESS, FALSE, EPARAMA)
#define SUCCEEDEDW SetErrorW(TRUE, SUCCESS, FALSE, EPARAMW)

typedef enum _ERROR_CODE {
	SUCCESS = 0,
	NULLPARAM,
	INVALIDPARAM,
	ALREADYREGISTERED,
	WINFUNCTION
} ERROR_CODE, * LPERROR_CODE;

typedef struct _ERRORA {
	ERROR_CODE nCode;
	CHAR szMessage[MSG_SIZE];
	DWORD dwWinCode;
	CHAR szWinMessage[MSG_SIZE];
	CHAR szFunction[FN_NAME_SIZE];
	DWORD dwLine;
	CHAR szFile[MAX_PATH];
} ERRORA, * LPERRORA;

typedef struct _ERRORW {
	ERROR_CODE nCode;
	WCHAR szMessage[MSG_SIZE];
	DWORD dwWinCode;
	WCHAR szWinMessage[MSG_SIZE];
	WCHAR szFunction[FN_NAME_SIZE];
	DWORD dwLine;
	WCHAR szFile[MAX_PATH];
} ERRORW, * LPERRORW;

typedef VOID(APIENTRY* ERRORHNDA)(LPERRORA);
typedef VOID(APIENTRY* ERRORHNDW)(LPERRORW);

BOOL APIENTRY SetErrorA(
	BOOL bReturn,
	ERROR_CODE nCode,
	BOOL bWin,
	LPCSTR lpFunction,
	DWORD dwLine,
	LPCSTR lpFile
);

BOOL APIENTRY SetErrorW(
	BOOL bReturn,
	ERROR_CODE nCode,
	BOOL bWin,
	LPCWSTR lpFunction,
	DWORD dwLine,
	LPCWSTR lpFile
);

LPERRORA APIENTRY GetErrorA();
LPERRORW APIENTRY GetErrorW();

#ifdef UNICODE

typedef ERRORW ERROR;
typedef LPERRORW LPERROR;
typedef ERRORHNDW ERRORHND;

#define SetError SetErrorW
#define GetError GetErrorW

#define EPARAM EPARAMW
#define THROW THROWW
#define SUCCEEDED SUCCEEDEDW

#else

typedef ERRORA ERROR;
typedef LPERRORA LPERROR;
typedef ERRORHNDA ERRORHND;

#define SetError SetErrorA
#define GetError GetErrorA

#define EPARAM EAPRAMA
#define THROW THROWA
#define SUCCEEDED SUCCEEDEDA

#endif // UNICODE

#endif // !__ERROR_H__