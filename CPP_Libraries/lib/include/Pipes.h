#ifndef LIB_PIPES_H
#define LIB_PIPES_H

#include <stdio.h>
#include <Windows.h>

#include "EnvVar.h"

LIB_BEGIN

enum class PipeAccess : DWORD {
	Inbound = PIPE_ACCESS_INBOUND, // Inbound, AKA read access.
	Outbound = PIPE_ACCESS_OUTBOUND, // Outbound, AKA write access.
	Duplex = PIPE_ACCESS_DUPLEX // In and outbound, AKA readwrite access.
};

enum class PipeMode : DWORD {
	MessagePipe = PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT | PIPE_ACCEPT_REMOTE_CLIENTS,
	DataPipe = PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT | PIPE_ACCEPT_REMOTE_CLIENTS
};

struct Pipe {
	HANDLE hReadPipe;
	HANDLE hWritePipe;
	DWORD dwSize;
	bool bIsOpen;
};

struct NamedPipe {
	char szName[MAX_PATH];
	HANDLE hPipe;
	DWORD dwSize;
	PipeAccess dwPipeAccess;
	bool bIsOpen;
};

bool CreatePipe(_Out_ Pipe* _Pipe, _In_ DWORD _Size);
bool CreatePipe(_Out_ NamedPipe* _Pipe, _In_ const char* _BaseName, _In_opt_ const char* _NameSuffix, _In_ PipeAccess _OpenMode, _In_ PipeMode _PipeMode, _In_ DWORD _Size);

bool ClosePipe(_In_ Pipe* _Pipe);
bool ClosePipe(_In_ NamedPipe* _Pipe);

bool OpenPipe(_Out_ NamedPipe* _Pipe, _In_ const char* _BaseName, _In_opt_ const char* _NameSuffix, _In_ PipeAccess _OpenMode);

template<typename _Ty>
bool ReadPipe(_In_ Pipe* _Pipe, _Out_writes_all_(_Size) _Ty* _Buffer, _In_ DWORD _Size) {
	if (!_Pipe) return false;
	if (_Size > _Pipe->dwSize) return false;

	return ReadFile(_Pipe->hReadPipe, _Buffer, _Size, nullptr, nullptr);
}

template<typename _Ty>
bool ReadPipe(_In_ NamedPipe* _Pipe, _Out_writes_all_(_Size) _Ty* _Buffer, _In_ DWORD _Size) {
	if (!_Pipe) return false;
	if (_Size > _Pipe->dwSize) return false;

	return ReadFile(_Pipe->hPipe, _Buffer, _Size, nullptr, nullptr);
}

template<typename _Ty>
bool WritePipe(_In_ Pipe* _Pipe, _In_ _Ty* _Buffer, _In_ DWORD _Size) {
	if (!_Pipe) return false;
	if (_Size > _Pipe->dwSize) return false;

	return WriteFile(_Pipe->hWritePipe, _Buffer, _Size, nullptr, nullptr);
}

template<typename _Ty>
bool WritePipe(_In_ NamedPipe* _Pipe, _In_ _Ty* _Buffer, _In_ DWORD _Size) {
	if (!_Pipe) return false;
	if (_Size > _Pipe->dwSize) return false;

	return WriteFile(_Pipe->hPipe, _Buffer, _Size, nullptr, nullptr);
}

LIB_END

#endif LIB_PIPES_H