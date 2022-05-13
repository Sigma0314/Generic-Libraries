#include "../include/Logging.h"

LIB_BEGIN

void Logger::LoggerEvent::Invoke(Logger* _This, LoggerParams* _Params, const char* _Prefix, const char* _Format, va_list _Args) {
	for (int i = 0; i < 128; i++)
		if (_HandlerArray[i])
			_HandlerArray[i](_This, _Params, _Prefix, _Format, _Args);
}

bool Logger::LoggerEvent::Add(tLoggerFunction _Handler) {
	if (_Count >= 128) return false;

	for (int i = 0; i < 128; i++)
		if (_HandlerArray[i] == _Handler)
			return false;

	for (int i = 0; i < 128; i++)
		if (!_HandlerArray[i]) {
			_HandlerArray[i] = _Handler;
			_Count++;
			return true;
		}

	return false;
}

bool Logger::LoggerEvent::Remove(tLoggerFunction _Handler) {
	for (int i = 0; i < 128; i++)
		if (_HandlerArray[i] == _Handler) {
			_HandlerArray[i] = nullptr;
			_Count--;
			return true;
		}
	return false;
};

LoggerParams* Logger::GetParams() {
	p = {
		isCEX,
		isConsole,
		isFile,
		isEvent,
		logTime,
		printColor,
		cexInst,
		hConsole,
		pFile
	};

	return &p;
}

void Logger::_BasicLogFile(FILE* pFile, bool bLogTime, const char* pPrefix, const char* pFormat, va_list pArgs) {
	if (bLogTime) {
		char szBuffer[64];
		GetSystemDateTime(szBuffer, 64);

		fprintf(pFile, "[%s] ", szBuffer);
	}

	fprintf(pFile, "[%s] ", pPrefix);
	vfprintf(pFile, pFormat, pArgs);
	fputc('\n', pFile);
}

void Logger::_BasicLogConsole(FILE* pStream, bool bLogTime, bool bPrintColor, HANDLE hOut, WORD wColor, const char* pPrefix, const char* pFormat, va_list pArgs) {
	if (bLogTime) {
		char szBuffer[64];
		GetSystemDateTime(szBuffer, 64);

		fprintf(pStream, "[%s] ", szBuffer);
	}

	if (bPrintColor) {
		fputc('[', pStream);
		SetConsoleTextAttribute(hOut, wColor);
		fprintf(pStream, "%s", pPrefix);
		SetConsoleTextAttribute(hOut, 7);
		fputs("] ", pStream);
	}
	else { fprintf(pStream, "[%s] ", pPrefix); }

	vfprintf(pStream, pFormat, pArgs);
	fputc('\n', pStream);
}

void Logger::_BasicLogCEX(ConsoleEx* pConsoleEx, bool bLogTime, bool bPrintColor, WORD wColor, const char* pPrefix, const char* pFormat, va_list pArgs) {
	if (bLogTime) {
		char szBuffer[64];
		GetSystemDateTime(szBuffer, 64);

		pConsoleEx->printf("[%s] ", szBuffer);
	}

	if (bPrintColor) {
		pConsoleEx->printf("[");
		// pConsoleEx->SetConsoleTextAttribute(wColor);
		// pConsoleEx->printf("%s", pPrefix);
		// pConsoleEx->SetConsoleTextAttribute(7);
		pConsoleEx->printfc(wColor, "%s", pPrefix);
		pConsoleEx->printf("] ");
	}
	else { pConsoleEx->printf("[%s] ", pPrefix); }

	pConsoleEx->vprintf(pFormat, pArgs);
	pConsoleEx->printf("\n");
}

Logger::Logger(bool _Console, bool _File, bool _Event, bool _PrintColor, bool _PrintTime) : isConsole(_Console), isFile(_File), isEvent(_Event), isCEX(false), printColor(_PrintColor), logTime(_PrintTime), cexInst(nullptr) {
	if (_Console) {
		hConsole = GetStdHandle(STD_ERROR_HANDLE);
		SetConsoleTextAttribute(hConsole, 7);
	}
	else hConsole = nullptr;

	if (_File) fopen_s(&pFile, "log.log", "w");
	else pFile = nullptr;
}

Logger::Logger(ConsoleEx* _ConsoleEx, bool _PrintColor, bool _PrintTime) : isConsole(false), isFile(false), isEvent(false), isCEX(true), printColor(_PrintColor), logTime(_PrintTime), cexInst(_ConsoleEx), hConsole(nullptr), pFile(nullptr) {
	_ConsoleEx->SetConsoleTextAttribute(7);
}

void Logger::LogSuccess(_In_z_ _Printf_format_string_ const char* _Format, ...) {
	va_list v1;
	va_start(v1, _Format);
	if (isConsole) _BasicLogConsole(stderr, logTime, printColor, hConsole, FOREGROUND_GREEN, "SUCCESS", _Format, v1);
	if (isFile) _BasicLogFile(pFile, logTime, "SUCCESS", _Format, v1);
	if (isCEX) _BasicLogCEX(cexInst, logTime, printColor, FOREGROUND_GREEN, "SUCCESS", _Format, v1);
	if (isEvent) this->SuccessEvent.Invoke(this, GetParams(), "SUCCESS", _Format, v1);

	va_end(v1);
}

void Logger::LogInfo(_In_z_ _Printf_format_string_ const char* _Format, ...) {
	va_list v1;
	va_start(v1, _Format);
	if (isConsole) _BasicLogConsole(stderr, logTime, printColor, hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY, "INFO", _Format, v1);
	if (isFile) _BasicLogFile(pFile, logTime, "INFO", _Format, v1);
	if (isCEX) _BasicLogCEX(cexInst, logTime, printColor, FOREGROUND_BLUE | FOREGROUND_INTENSITY, "INFO", _Format, v1);
	// Insert event.

	va_end(v1);
}

void Logger::LogWarning(_In_z_ _Printf_format_string_ const char* _Format, ...) {
	va_list v1;
	va_start(v1, _Format);
	if (isConsole) _BasicLogConsole(stderr, logTime, printColor, hConsole, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY, "WARNING", _Format, v1);
	if (isFile) _BasicLogFile(pFile, logTime, "WARNING", _Format, v1);
	if (isCEX) _BasicLogCEX(cexInst, logTime, printColor, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY, "WARNING", _Format, v1);
	// Insert event.

	va_end(v1);
}

void Logger::LogError(_In_z_ _Printf_format_string_ const char* _Format, ...) {
	va_list v1;
	va_start(v1, _Format);
	if (isConsole) _BasicLogConsole(stderr, logTime, printColor, hConsole, FOREGROUND_RED, "ERROR", _Format, v1);
	if (isFile) _BasicLogFile(pFile, logTime, "ERROR", _Format, v1);
	if (isCEX) _BasicLogCEX(cexInst, logTime, printColor, FOREGROUND_RED, "ERROR", _Format, v1);
	// Insert event.

	va_end(v1);
}

bool Logger::AddEventFunction(_In_ tLoggerFunction _Function, LogLevel _Level) {
	switch (_Level) {
	case LogLevel::Success:
		return SuccessEvent.Add(_Function);
	case LogLevel::Info:
		return InfoEvent.Add(_Function);
	case LogLevel::Warning:
		return WarningEvent.Add(_Function);
	case LogLevel::Error:
		return ErrorEvent.Add(_Function);
	case LogLevel::All:
		return SuccessEvent.Add(_Function) && InfoEvent.Add(_Function) && WarningEvent.Add(_Function) && ErrorEvent.Add(_Function);
	}
}

bool Logger::RemoveEventFunction(_In_ tLoggerFunction _Function, LogLevel _Level) {
	switch (_Level) {
	case LogLevel::Success:
		return SuccessEvent.Remove(_Function);
	case LogLevel::Info:
		return InfoEvent.Remove(_Function);
	case LogLevel::Warning:
		return WarningEvent.Remove(_Function);
	case LogLevel::Error:
		return ErrorEvent.Remove(_Function);
	case LogLevel::All:
		return SuccessEvent.Remove(_Function) && InfoEvent.Remove(_Function) && WarningEvent.Remove(_Function) && ErrorEvent.Remove(_Function);
	}
}

LIB_END