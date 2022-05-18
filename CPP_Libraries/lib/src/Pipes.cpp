#include "../include/Pipes.h"

LIB_BEGIN

bool CreatePipe(_Out_ Pipe* _Pipe, _In_ DWORD _Size) {
	if (!_Pipe) return false;

	if (!CreatePipe(&_Pipe->hReadPipe, &_Pipe->hWritePipe, (LPSECURITY_ATTRIBUTES)nullptr, _Size)) return false;

	_Pipe->dwSize = _Size;
	_Pipe->bIsOpen = true;
	return true;
}

bool CreatePipe(_Out_ NamedPipe* _Pipe, _In_ const char* _BaseName, _In_opt_ const char* _NameSuffix, _In_ PipeAccess _OpenMode, _In_ PipeMode _PipeMode, _In_ DWORD _Size) {
	if (!_Pipe) return false;

	char szBuffer[MAX_PATH];
	sprintf_s(szBuffer, "\\\\.\\pipe\\%s%s", _BaseName, _NameSuffix ? _NameSuffix : "");

	_Pipe->hPipe = CreateNamedPipeA(
		szBuffer,
		(DWORD)_OpenMode,
		(DWORD)_PipeMode,
		PIPE_UNLIMITED_INSTANCES,
		_Size,
		_Size,
		INFINITE,
		nullptr
	);

	if (_Pipe->hPipe == INVALID_HANDLE_VALUE) return false;

	sprintf_s(_Pipe->szName, "%s%s", _BaseName, _NameSuffix ? _NameSuffix : "");
	_Pipe->dwPipeAccess = _OpenMode;
	_Pipe->dwSize = _Size;
	_Pipe->bIsOpen = true;

	return true;
}

bool ClosePipe(_In_ Pipe* _Pipe) {
	if (!_Pipe) return false;
	if (!_Pipe->bIsOpen) return true;

	_Pipe->bIsOpen = !(CloseHandle(_Pipe->hReadPipe) && CloseHandle(_Pipe->hWritePipe));
	return _Pipe->bIsOpen;
}

bool ClosePipe(_In_ NamedPipe* _Pipe) {
	if (!_Pipe) return false;
	if (!_Pipe->bIsOpen) return true;

	if (!CloseHandle(_Pipe->hPipe)) return false;

	_Pipe->bIsOpen = false;
	return true;
}

bool OpenPipe(_Out_ NamedPipe* _Pipe, _In_ const char* _BaseName, _In_opt_ const char* _NameSuffix, _In_ PipeAccess _OpenMode) {
	if (!_Pipe) return false;

	char szBuffer[MAX_PATH];
	sprintf_s(szBuffer, "\\\\.\\pipe\\%s%s", _BaseName, _NameSuffix ? _NameSuffix : "");

	_Pipe->hPipe = CreateFileA(
		szBuffer,
		(DWORD)(_OpenMode == PipeAccess::Duplex ? GENERIC_READ | GENERIC_WRITE : (_OpenMode == PipeAccess::Inbound ? GENERIC_READ : GENERIC_WRITE)),
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);

	if (_Pipe->hPipe == INVALID_HANDLE_VALUE) return false;

	sprintf_s(_Pipe->szName, "%s%s", _BaseName, _NameSuffix ? _NameSuffix : "");
	_Pipe->dwSize = -1;
	_Pipe->dwPipeAccess = _OpenMode;
	_Pipe->bIsOpen = true;
	return true;
}

LIB_END