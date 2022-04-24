#ifndef LIB_LOGGING_H
#define LIB_LOGGING_H

#include <stdio.h>		// FILE, va_list, fprintf, stderr, vfprintf, fputc, 
#include <Windows.h>	// SetConsoleTextAttribute, WORD, HANDLE, SYSTEMTIME, GetSystemTime

#include "EnvVar.h"
#include "Error.h"		// Error

#define LOGFN_OVERRIDE(level) void __fastcall Log##level##Override(lib::Logger* _This, const char* _Format, va_list _Args)

LIB_BEGIN

enum class LogLevel {
	Success,
	Info,
	Warning,
	Error
};

enum class ColorSetting {
	PrefixOnly,
	TextOnly,
	PrefixAndTime,
	All
};

class Logger {
public:

	Logger() :
		_DefaultLogLevel(LogLevel::Info), _ColorSetting(ColorSetting::PrefixOnly), _LogToStdErr(false), _LogToFile(false), _LogTime(false), _LogFile(nullptr), hConsole(nullptr),
		_LogSuccessOverride(nullptr), _LogInfoOverride(nullptr), _LogWarningOverride(nullptr), _LogErrorOverride(nullptr) {}
	Logger(
		_In_ bool _LogToConsole,
		_In_ bool _LogToFile,
		_In_opt_ LogLevel _DefaultLogLevel = LogLevel::Info,
		_In_opt_ ColorSetting _ColorSetting = ColorSetting::PrefixOnly,
		_In_opt_ bool _LogTime = true
	);
	~Logger();

	void Log(_In_z_ _Printf_format_string_ const char* _Format, ...);
	void LogSuccess(_In_z_ _Printf_format_string_ const char* _Format, ...);
	void LogInfo(_In_z_ _Printf_format_string_ const char* _Format, ...);
	void LogWarning(_In_z_ _Printf_format_string_ const char* _Format, ...);
	void LogError(_In_z_ _Printf_format_string_ const char* _Format, ...);

	void OverrideLogFunction(_In_opt_ void(__fastcall* _LogOverride)(Logger*, const char*, va_list), _In_ LogLevel _Level);

private:
	LogLevel _DefaultLogLevel;
	ColorSetting _ColorSetting;
	bool _LogToStdErr;
	bool _LogToFile;
	bool _LogTime;
	FILE* _LogFile;
	HANDLE hConsole;

	void(__fastcall* _LogSuccessOverride)(Logger* _This, const char* _Format, va_list _Args);
	void(__fastcall* _LogInfoOverride)(Logger* _This, const char* _Format, va_list _Args);
	void(__fastcall* _LogWarningOverride)(Logger* _This, const char* _Format, va_list _Args);
	void(__fastcall* _LogErrorOverride)(Logger* _This, const char* _Format, va_list _Args);

	void _Log(const char* _Prefix, WORD _ConsoleAttribute, const char* _Format, va_list _Args);
	void __fastcall _LogSuccess(const char* _Format, va_list _Args);
	void __fastcall _LogInfo(const char* _Format, va_list _Args);
	void __fastcall _LogWarning(const char* _Format, va_list _Args);
	void __fastcall _LogError(const char* _Format, va_list _Args);
};

LIB_END

#endif