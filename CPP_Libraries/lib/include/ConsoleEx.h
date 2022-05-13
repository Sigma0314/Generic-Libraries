#ifndef LIB_CONSOLEEX_H
#define LIB_CONSOLEEX_H

#include <stdio.h>
#include <Windows.h>
#include <Psapi.h>

#include "EnvVar.h"
#include "Pipes.h"

LIB_BEGIN

class ConsoleEx {
private:
	char szName[64];
	lib::NamedPipe msgsend; // Server outbound. // "msgsend"
	lib::NamedPipe msgrec; // Server inbound. // "msgrec"
	lib::NamedPipe prntout; // To be printed. Outbound. // "prntout"
	lib::NamedPipe scanfmt; // scanf formats. Outbound. // "scanfmt"
	lib::NamedPipe scanfres; // scanf result. Inbound. // "scanfres"

	HANDLE hThread;

	enum class RM_INTENT: BYTE{
		READY = 0x01
	};

	enum class SM_INTENT : BYTE {
		TERMINATE = 0x00,
		READY = 0x01,
		PRINT = 0x02,
		COLOR = 0x03,
		PRINTC = 0x04
	};

	static void StartProcess(const char* _BaseName);
	
	template<typename _Ty>
	static bool _PostMessage(lib::NamedPipe* _Pipe, SM_INTENT _Intent, _Ty* _ParamPtr, DWORD _Size) {
		if (!_Pipe) return false;
		if (_Size + 1 > _Pipe->dwSize) return false;

		BYTE* v = new BYTE[_Size + 1];

		v[0] = (BYTE)_Intent;
		if (_Size) memcpy(&v[1], _ParamPtr, _Size);

		bool ret = lib::WritePipe(_Pipe, v, _Size + 1);

		delete[] v;

		return ret;
	}
public:
	ConsoleEx(_In_ const char* _Name);
	~ConsoleEx();

	void printf(_In_z_ _Printf_format_string_ const char* _Format, ...);
	void printfc(WORD wColor, _In_z_ _Printf_format_string_ const char* _Format, ...);
	void vprintf(_In_z_ _Printf_format_string_ const char* _Format, va_list _Args);
	void SetConsoleTextAttribute(WORD wAttribute);
};

LIB_END

#endif
