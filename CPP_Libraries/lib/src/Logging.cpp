#include "../include/Logging.h"

LIB_BEGIN

void Logger::_Log(const char* _Prefix, WORD _ConsoleAttribute, const char* _Format, va_list _Args) {
	if (this->_ColorSetting == ColorSetting::All) SetConsoleTextAttribute(this->hConsole, _ConsoleAttribute);

	if (this->_LogTime) {
		SYSTEMTIME st;
		GetSystemTime(&st);

		if (this->_ColorSetting != ColorSetting::All && this->_ColorSetting == ColorSetting::PrefixAndTime) {
			if (this->_LogToStdErr) {
				fprintf(stderr, "[");
				SetConsoleTextAttribute(this->hConsole, _ConsoleAttribute);
				fprintf(stderr, "%hi-%hi-%hi %hi:%hi:%hi", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
				SetConsoleTextAttribute(this->hConsole, 7);
				fprintf(stderr, "] ");
			}

			if (this->_LogToFile) fprintf(this->_LogFile, "[%hi-%hi-%hi %hi:%hi:%hi] ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		}

		else {
			if (this->_LogToStdErr) fprintf(stderr, "[%hi-%hi-%hi %hi:%hi:%hi] ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			if (this->_LogToFile) fprintf(this->_LogFile, "[%hi-%hi-%hi %hi:%hi:%hi] ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		}
	}

	if (this->_ColorSetting != ColorSetting::All && (this->_ColorSetting == ColorSetting::PrefixOnly || this->_ColorSetting == ColorSetting::PrefixAndTime)) {
		if (this->_LogToStdErr) {
			fprintf(stderr, "[");
			SetConsoleTextAttribute(this->hConsole, _ConsoleAttribute);
			fprintf(stderr, "%s", _Prefix);
			SetConsoleTextAttribute(this->hConsole, 7);
			fprintf(stderr, "] ");
		}

		if (this->_LogToFile) fprintf(this->_LogFile, "[%s] ", _Prefix);
	}

	else {
		if (this->_LogToStdErr) fprintf(stderr, "[%s] ", _Prefix);
		if (this->_LogToFile) fprintf(this->_LogFile, "[%s] ", _Prefix);
	}

	if (this->_ColorSetting != ColorSetting::All && this->_ColorSetting == ColorSetting::TextOnly) {
		if (this->_LogToStdErr) {
			SetConsoleTextAttribute(this->hConsole, _ConsoleAttribute);
			vfprintf(stderr, _Format, _Args);
			SetConsoleTextAttribute(this->hConsole, 7);
		}

		if (this->_LogToFile) vfprintf(this->_LogFile, _Format, _Args);
	}

	else {
		if (this->_LogToStdErr) vfprintf(stderr, _Format, _Args);
		if (this->_LogToFile) vfprintf(this->_LogFile, _Format, _Args);
	}

	if (this->_LogToStdErr) fputc('\n', stderr);
	if (this->_LogToFile) fputc('\n', this->_LogFile);
}

void Logger::_LogSuccess(const char* _Format, va_list _Args) { this->_Log("SUCCESS", FOREGROUND_GREEN, _Format, _Args); }
	 
void Logger::_LogInfo(const char* _Format, va_list _Args) { this->_Log("INFO", FOREGROUND_BLUE | FOREGROUND_INTENSITY, _Format, _Args); }
	 
void Logger::_LogWarning(const char* _Format, va_list _Args) { this->_Log("WARNING", FOREGROUND_RED | FOREGROUND_GREEN, _Format, _Args); }
	 
void Logger::_LogError(const char* _Format, va_list _Args) { this->_Log("ERROR", FOREGROUND_RED, _Format, _Args); }

Logger::Logger(
	_In_ bool _LogToConsole,
	_In_ bool _LogToFile,
	_In_opt_ LogLevel _DefaultLogLevel,
	_In_opt_ ColorSetting _ColorSetting,
	_In_opt_ bool _LogTime
) : _DefaultLogLevel(_DefaultLogLevel), _ColorSetting(_ColorSetting), _LogToStdErr(_LogToConsole), _LogToFile(_LogToFile), _LogTime(_LogTime), _LogSuccessOverride(nullptr), _LogInfoOverride(nullptr), _LogWarningOverride(nullptr), _LogErrorOverride(nullptr) {
	if (_LogToConsole) {
		this->hConsole = GetStdHandle(STD_ERROR_HANDLE);
		SetConsoleTextAttribute(this->hConsole, 7);
	}
	else this->hConsole = nullptr;

	if (_LogToFile) fopen_s(&this->_LogFile, "log.log", "w");
	else this->_LogFile = nullptr;
}

Logger::~Logger() { if (this->_LogToFile) fclose(this->_LogFile); }

void Logger::Log(_In_z_ _Printf_format_string_ const char* _Format, ...) {
	va_list v1;
	va_start(v1, _Format);
	switch (this->_DefaultLogLevel) {
	case LogLevel::Success:
		this->_LogSuccess(_Format, v1);
		break;
	case LogLevel::Info:
		this->_LogInfo(_Format, v1);
		break;
	case LogLevel::Warning:
		this->_LogWarning(_Format, v1);
		break;
	case LogLevel::Error:
		this->_LogError(_Format, v1);
		break;
	default:
		break;
	}
	va_end(v1);
}

void Logger::LogSuccess(_In_z_ _Printf_format_string_ const char* _Format, ...) {
	va_list v1;
	va_start(v1, _Format);
	if (this->_LogSuccessOverride) this->_LogSuccessOverride(this, _Format, v1);
	else this->_LogSuccess(_Format, v1);
	va_end(v1);
}

void Logger::LogInfo(_In_z_ _Printf_format_string_ const char* _Format, ...) {
	va_list v1;
	va_start(v1, _Format);
	if (this->_LogInfoOverride) this->_LogInfoOverride(this, _Format, v1);
	else this->_LogInfo(_Format, v1);
	va_end(v1);
}

void Logger::LogWarning(_In_z_ _Printf_format_string_ const char* _Format, ...) {
	va_list v1;
	va_start(v1, _Format);
	if (this->_LogWarningOverride) this->_LogWarningOverride(this, _Format, v1);
	else this->_LogWarning(_Format, v1);
	va_end(v1);
}

void Logger::LogError(_In_z_ _Printf_format_string_ const char* _Format, ...) {
	va_list v1;
	va_start(v1, _Format);
	if (this->_LogErrorOverride) this->_LogErrorOverride(this, _Format, v1);
	else this->_LogError(_Format, v1);
	va_end(v1);
}

void Logger::OverrideLogFunction(_In_opt_ void(__fastcall* _LogOverride)(Logger*, const char*, va_list), _In_ LogLevel _Level) {
	switch (_Level) {
	case LogLevel::Success:
		this->_LogSuccessOverride = _LogOverride;
		return;
	case LogLevel::Info:
		this->_LogInfoOverride = _LogOverride;
		return;
	case LogLevel::Warning:
		this->_LogWarningOverride = _LogOverride;
		return;
	case LogLevel::Error:
		this->_LogErrorOverride = _LogOverride;
		return;
	default:
		return;
	}
}

LIB_END