#include <stdio.h>
#include <Windows.h>

#include "../CPP_Libraries/lib/include/Pipes.h"

class ConsoleExClient {
private:
	bool* _Switch;

	lib::NamedPipe msgsend; // Client inbound.
	lib::NamedPipe msgrec; // Client outbound.
	lib::NamedPipe prntout; // To be printed. Inbound.
	lib::NamedPipe scanfmt; // scanf formats. Inbound.
	lib::NamedPipe scanfres; // scanf result. Outbound.

	struct ThreadParam {
		lib::NamedPipe* lpPipes[5];
		bool* lpSwitchPtr;
	};

	enum class RM_INTENT : BYTE {
		TERMINATE = 0x00,
		READY = 0x01,
		PRINT = 0x02,
		COLOR = 0x03,
		PRINTC = 0x04
	};

	enum class SM_INTENT : BYTE {
		READY = 0x01
	};

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

	static void printfn(lib::NamedPipe* p) {
		char szBuffer[256];
		ReadPipe(p, szBuffer, sizeof(szBuffer));

		printf("%s", szBuffer);
	}

	static void MsgThread(ThreadParam* p) {
		BYTE buf[64];
		lib::NamedPipe* msgsend = p->lpPipes[0];
		lib::NamedPipe* msgrec = p->lpPipes[1];
		lib::NamedPipe* prntout = p->lpPipes[2];
		lib::NamedPipe* scanfmt = p->lpPipes[3];
		lib::NamedPipe* scanres = p->lpPipes[4];
		bool* _SwitchPtr = p->lpSwitchPtr;

		delete p;

		while (lib::ReadPipe(msgsend, buf, sizeof(buf))) {
			switch ((RM_INTENT)buf[0]) {
			case RM_INTENT::TERMINATE:
				goto _exit;
			case RM_INTENT::READY:
				_PostMessage(msgrec, SM_INTENT::READY, (int*)nullptr, NULL);
				break;
			case RM_INTENT::PRINT:
				printfn(prntout);
				break;
			case RM_INTENT::COLOR:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), *(WORD*)&buf[1]);
				break;
			case RM_INTENT::PRINTC:
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), *(WORD*)&buf[1]);
				printfn(prntout);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
				break;
			}
		}

	_exit:;
		*_SwitchPtr = false;
	}

public:
	ConsoleExClient(const char* _Name, bool* _KillSwitch) : _Switch(_KillSwitch) {
		lib::OpenPipe(&msgsend, _Name, "msgsend", lib::PipeAccess::Inbound);
		lib::OpenPipe(&msgrec, _Name, "msgrec", lib::PipeAccess::Outbound);
		lib::OpenPipe(&prntout, _Name, "prntout", lib::PipeAccess::Inbound);
		lib::OpenPipe(&scanfmt, _Name, "scanfmt", lib::PipeAccess::Inbound);
		lib::OpenPipe(&scanfres, _Name, "scanfres", lib::PipeAccess::Outbound);

		ThreadParam* p = new ThreadParam;
		p->lpPipes[0] = &msgsend;
		p->lpPipes[1] = &msgrec;
		p->lpPipes[2] = &prntout;
		p->lpPipes[3] = &scanfmt;
		p->lpPipes[4] = &scanfres;
		p->lpSwitchPtr = _KillSwitch;

		CloseHandle(CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)MsgThread, p, NULL, nullptr));
	}

	~ConsoleExClient() {
		lib::ClosePipe(&msgsend);
		lib::ClosePipe(&msgrec);
		lib::ClosePipe(&prntout);
		lib::ClosePipe(&scanfmt);
		lib::ClosePipe(&scanfres);
	}
};

int main(int argc, char** argv) {
	if (argc != 2) return -1;

	bool kswitch = true;
	ConsoleExClient cexc(argv[1], &kswitch);

	while (kswitch);

	return NULL;
}