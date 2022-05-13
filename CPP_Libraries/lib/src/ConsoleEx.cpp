#include "../include/ConsoleEx.h"

LIB_BEGIN

bool b = true;

void ConsoleEx::StartProcess(const char* _BaseName) {
	char szPath[MAX_PATH];
	K32GetModuleFileNameExA(GetCurrentProcess(), GetModuleHandleA(nullptr), szPath, sizeof(szPath));

	for (int i = strlen(szPath); szPath[i] != '\\'; szPath[i--] = '\0');

	char szBuffer[256];
	sprintf_s(szBuffer, "start /i %sConsoleExProc.exe %s", szPath, _BaseName);
	system(szBuffer);
}

ConsoleEx::ConsoleEx(_In_ const char* _Name) {
	strcpy_s(szName, _Name);

	CreatePipe(&msgsend, _Name, "msgsend", PipeAccess::Outbound, PipeMode::MessagePipe, 64);
	CreatePipe(&msgrec, _Name, "msgrec", PipeAccess::Inbound, PipeMode::MessagePipe, 64);
	CreatePipe(&prntout, _Name, "prntout", PipeAccess::Outbound, PipeMode::MessagePipe, 256);
	CreatePipe(&scanfmt, _Name, "scanfmt", PipeAccess::Outbound, PipeMode::MessagePipe, 256);
	CreatePipe(&scanfres, _Name, "scanfres", PipeAccess::Inbound, PipeMode::MessagePipe, 256);

	StartProcess(_Name);
	Sleep(200);
	_PostMessage(&msgsend, SM_INTENT::READY, (int*)nullptr, NULL);
	BYTE b;
	ReadPipe(&msgrec, &b, 1);
	if ((RM_INTENT)b != RM_INTENT::READY) return;
}

ConsoleEx::~ConsoleEx() {
	_PostMessage(&msgsend, SM_INTENT::TERMINATE, (int*)nullptr, NULL);

	b = false;

	ClosePipe(&msgsend);
	ClosePipe(&msgrec);
	ClosePipe(&prntout);
	ClosePipe(&scanfmt);
	ClosePipe(&scanfres);
}

void ConsoleEx::printf(_In_z_ _Printf_format_string_ const char* _Format, ...) {
	va_list v1;
	va_start(v1, _Format);
	this->vprintf(_Format, v1);
	va_end(v1);
}

void ConsoleEx::printfc(WORD wColor, _In_z_ _Printf_format_string_ const char* _Format, ...) {
	va_list v1;
	va_start(v1, _Format);
	char szBuffer[256];
	vsprintf_s(szBuffer, _Format, v1);
	va_end(v1);

	WritePipe(&prntout, szBuffer, sizeof(szBuffer));
	_PostMessage(&msgsend, SM_INTENT::PRINTC, &wColor, sizeof(WORD));
}

void ConsoleEx::vprintf(_In_z_ _Printf_format_string_ const char* _Format, va_list _Args) {
	char szBuffer[256];
	vsprintf_s(szBuffer, _Format, _Args);

	WritePipe(&prntout, szBuffer, sizeof(szBuffer));
	_PostMessage(&msgsend, SM_INTENT::PRINT, (int*)nullptr, NULL);
}

void ConsoleEx::SetConsoleTextAttribute(WORD wAttribute) { _PostMessage(&msgsend, SM_INTENT::COLOR, &wAttribute, sizeof(WORD)); }

LIB_END