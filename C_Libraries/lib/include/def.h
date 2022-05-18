#ifndef __DEF_H__
#define __DEF_H__

#define VOID				void

#define STDCALL				__stdcall
#define CDECL				__cdecl
#define APIENTRY			STDCALL

#define TRUE				1
#define FALSE				0

typedef unsigned short		WORD,		* LPWORD;
typedef unsigned long		DWORD,		* LPDWORD;
typedef unsigned long long	QWORD,		* LPQWORD;

typedef char				CHAR,		* LPCHAR;
typedef short				SHORT,		* LPSHORT;
typedef int					BOOL,		* LPBOOL;
typedef int					INT,		* LPINT;
typedef long				LONG,		* LPLONG;
typedef long long			LLONG,		* LPLLONG;

typedef unsigned char		BYTE,		* LPBYTE;
typedef unsigned short		WCHAR,		* LPWCHAR;
typedef unsigned short		USHORT,		* LPUSHORT;
typedef unsigned int		UINT,		* LPUINT;
typedef unsigned long		ULONG,		* LPULONG;
typedef unsigned long long	ULLONG,		* LPULLONG;
typedef unsigned long long	SIZE_T,		* LPSIZE_T;

typedef char				INT8,		* LPINT8;
typedef short				INT16,		* LPINT16;
typedef int					INT32,		* LPINT32;
typedef long long			INT64,		* LPINT64;

typedef unsigned char		UINT8,		* LPUINT8;
typedef unsigned short		UINT16,		* LPUINT16;
typedef unsigned int		UINT32,		* LPUINT32;
typedef unsigned long long	UINT64,     * LPUINT64;

typedef	CHAR*				LPSTR;
typedef WCHAR*				LPWSTR;
typedef VOID*				LPVOID;

typedef const CHAR*			LPCSTR;
typedef const WCHAR*		LPCWSTR;
typedef const VOID*			LPCVOID;

typedef VOID*				HANDLE;

#ifdef UNICODE
	typedef WCHAR			TCHAR,		* LPTCHAR;
	typedef LPWSTR			LPTSTR;
	typedef LPCWSTR			LPCTSTR;
#else
	typedef CHAR			TCHAR,		* LPTCHAR;
	typedef LPSTR			LPTSTR;
	typedef LPCSTR			LPCTSTR;
#endif // UNICODE

#ifdef _WIN64
	typedef UINT64			ADDRESS,	* LPADDRESS;
#else
	typedef UINT32			ADDRESS,	* LPADDRESS;
#endif // _WIN64

#endif // !__DEF_H__