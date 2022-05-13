#ifndef LIB_LOGGING_H
#define LIB_LOGGING_H

#include <stdio.h>
#include <Windows.h>

#include "EnvVar.h"
#include "ConsoleEx.h"
#include "Misc.h"

LIB_BEGIN

enum class LogLevel { Success, Info, Warning, Error, All };

struct LoggerParams {
	bool isCEX;
	bool isConsole;
	bool isFile;
	bool isEvent;

	bool bPrintTime;
	bool bPrintColor;

	ConsoleEx* pConsoleExInstance;
	HANDLE hConsole;
	FILE* pFile;
};

class Logger {
public:
	typedef void(__cdecl* tLoggerFunction)(Logger*, LoggerParams*, const char* _Prefix, const char* _Format, va_list _Args);

private:
	class LoggerEvent {
		tLoggerFunction _HandlerArray[128];
		size_t _Count;

	public:
		inline LoggerEvent() : _Count(0) { memset(_HandlerArray, NULL, sizeof(_HandlerArray)); }
		void Invoke(Logger* _This, LoggerParams* _Params, const char* _Prefix, const char* _Format, va_list _Args);
		bool Add(tLoggerFunction _Handler);
		bool Remove(tLoggerFunction _Handler);
	};

	bool isCEX;
	bool isConsole;
	bool isFile;
	bool isEvent;

	bool logTime; // ALL
	bool printColor; // CEX & Console

	ConsoleEx* cexInst; // CEX
	HANDLE hConsole; // Console
	FILE* pFile; // File

	LoggerEvent SuccessEvent;
	LoggerEvent InfoEvent;
	LoggerEvent WarningEvent;
	LoggerEvent ErrorEvent;

	LoggerParams p;
	LoggerParams* GetParams();

	static void _BasicLogFile(FILE* pFile, bool bLogTime, const char* pPrefix, const char* pFormat, va_list pArgs);
	static void _BasicLogConsole(FILE* pStream, bool bLogTime, bool bPrintColor, HANDLE hOut, WORD wColor, const char* pPrefix, const char* pFormat, va_list pArgs);
	static void _BasicLogCEX(ConsoleEx* pConsoleEx, bool bLogTime, bool bPrintColor, WORD wColor, const char* pPrefix, const char* pFormat, va_list pArgs);

public:
	Logger() : isCEX(false), isConsole(false), isFile(false), isEvent(false), logTime(false), printColor(false), cexInst(nullptr), hConsole(nullptr), pFile(nullptr) {}; // 0-arg ctor.
	Logger(bool _Console, bool _File, bool _Event, bool _PrintColor = true, bool _PrintTime = true); // Normal ctor.
	Logger(ConsoleEx* _ConsoleEx, bool _PrintColor = true, bool _PrintTime = true); // ConsoleEx ctor.

	void LogSuccess(_In_z_ _Printf_format_string_ const char* _Format, ...);
	void LogInfo(_In_z_ _Printf_format_string_ const char* _Format, ...);
	void LogWarning(_In_z_ _Printf_format_string_ const char* _Format, ...);
	void LogError(_In_z_ _Printf_format_string_ const char* _Format, ...);

	bool AddEventFunction(_In_ tLoggerFunction _Function, LogLevel _Level);
	bool RemoveEventFunction(_In_ tLoggerFunction _Function, LogLevel _Level);
};

LIB_END

#endif