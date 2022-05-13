#ifndef __TSTDIO_H__
#define __TSTDIO_H__

#include <stdio.h>

#ifdef UNICODE

#define TEXT(quote) L ## quote

#define tprintf wprintf
#define stprintf swprintf
#define stprintf_s swprintf_s
#define ftprintf fwprintf

#define tscanf wscanf
#define tscanf_s wscanf_s
#define stscanf swscanf
#define stscanf_s swscanf_s
#define ftscanf fwscanf
#define ftscanf_s fwscanf_s

#else

#define TEXT(quote) quote

#define tprintf printf
#define stprintf sprintf
#define stprintf_s sprintf_s
#define ftprintf fprintf

#define tscanf scanf
#define tscanf_s scanf_s
#define stscanf sscanf
#define stscanf_s sscanf_s
#define ftscanf fscanf
#define ftscanf_s fscanf_s

#endif

#endif // !__TSTDIO_H__