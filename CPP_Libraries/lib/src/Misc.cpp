#include "../include/Misc.h"

LIB_BEGIN

bool GetSystemDateTime(
	_Out_writes_all_(_Size) char* _Buffer,
	_In_ size_t _Size,
	_In_ const char* _Format
) {
	_Buffer[0] = '\0';

	size_t fmtLen = strlen(_Format);
	if (fmtLen >= _Size) return false;

	char buf[64] = { '\0' };
	bool skip = false;

	WORD wCur;

	SYSTEMTIME st;
	GetSystemTime(&st);

	for (size_t i = 0; i < fmtLen; i++) {
		if (_Format[i] == '%') {
			switch (_Format[i + 1]) {
			case 'i': wCur = st.wMilliseconds; break;
			case 's': wCur = st.wSecond; break;
			case 'm': wCur = st.wMinute; break;
			case 'h': wCur = st.wHour; break;
			case 'd': wCur = st.wDay; break;
			case 'w': wCur = st.wDayOfWeek; break;
			case 't': wCur = st.wDayOfWeek;
				skip = true;

				strcat_s(_Buffer, _Size,
					wCur == 1 ? "Monday" : (
						wCur == 2 ? "Tuesday" : (
							wCur == 3 ? "Wednesday" : (
								wCur == 4 ? "Thursday" : (
									wCur == 5 ? "Friday" : (
										wCur == 6 ? "Saturday" : (
											wCur == 7 ? "Sunday" : ("Invalid"))))))));
				break;
			case 'o': wCur = st.wMonth; break;
			case 'y': wCur = st.wYear; break;
			default: return false;
			}

			if (!skip) {
				sprintf_s(buf, "%hu", wCur);
				strcat_s(_Buffer, _Size, buf);
			}
			else skip = false;

			i++;
		}
		else {
			size_t len = strlen(_Buffer);
			_Buffer[len] = _Format[i];
			_Buffer[len + 1] = '\0';
		}
	}
	return true;
}

bool InjectDll(
	_In_ const wchar_t* _TargetProcess,
	_In_ const char* _LibraryFile,
	_In_ PathRelativity _PathRelativity,
	_In_ Architecture _Architecture,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, FALSE);
		if (hSnap == INVALID_HANDLE_VALUE) throw Error(ERRORDEF, Error::GenericWinApiFunctionError, true);

		PROCESSENTRY32W pe32;
		pe32.dwSize = sizeof(pe32);

		if (!Process32FirstW(hSnap, &pe32)) throw Error(ERRORDEF, Error::GenericWinApiFunctionError, true);

		DWORD dwProcessId = NULL;
		do {

			if (!_wcsicmp(_TargetProcess, pe32.szExeFile)) {
				dwProcessId = pe32.th32ProcessID;
				break;
			}

		} while (Process32NextW(hSnap, &pe32));
		CloseHandle(hSnap);

		if (!dwProcessId) throw Error(ERRORDEF, Error::THEntryNotFoundError);

		HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD, FALSE, dwProcessId);
		if (!hProcess) throw Error(ERRORDEF, Error::GenericWinApiFunctionError, true);

		BOOL bWow64;
		if (!IsWow64Process(hProcess, &bWow64)) throw Error(ERRORDEF, Error::GenericWinApiFunctionError, { hProcess }, true);

		if (_Architecture == Architecture::X64)
			if (bWow64) throw Error(ERRORDEF, Error::ArchitectureMismatchError, { hProcess }, false);
			else;
		else if (_Architecture == Architecture::X86)
			if (!bWow64) throw Error(ERRORDEF, Error::ArchitectureMismatchError, { hProcess }, false);
			else;
		else throw Error(ERRORDEF, Error::ParameterInvalidError, { hProcess }, false);

		CHAR szRealPath[MAX_PATH] = { '\0' };
		if (_PathRelativity == PathRelativity::ToTarget) {
			if (!K32GetModuleFileNameExA(hProcess, nullptr, szRealPath, sizeof(szRealPath))) throw Error(ERRORDEF, Error::GenericWinApiFunctionError, { hProcess }, true);
			for (; szRealPath[strlen(szRealPath) - 1] != '\\'; szRealPath[strlen(szRealPath) - 1] = '\0');
			strcat_s(szRealPath, _LibraryFile);
		}
		else if (_PathRelativity == PathRelativity::ToCaller) {
			if (!GetModuleFileNameA(nullptr, szRealPath, MAX_PATH)) throw Error(ERRORDEF, Error::GenericWinApiFunctionError, { hProcess }, true);
			for (; szRealPath[strlen(szRealPath) - 1] != '\\'; szRealPath[strlen(szRealPath) - 1] = '\0');
			strcat_s(szRealPath, _LibraryFile);
		}
		else if (_PathRelativity == PathRelativity::Absolute) {
			strcpy_s(szRealPath, _LibraryFile);
		}
		else throw Error(ERRORDEF, Error::ParameterInvalidError, { hProcess }, false);

		FILE* f;
		fopen_s(&f, szRealPath, "r");
		if (!f) throw Error(ERRORDEF, Error::InvalidFilePathError, { hProcess }, false);
		fclose(f);

		size_t nStrLen = strlen(szRealPath) + 1;

		HMODULE hModule = GetModuleHandleA("kernel32.dll");
		if (!hModule) throw Error(ERRORDEF, Error::GenericWinApiFunctionError, { hProcess }, true);

		LPTHREAD_START_ROUTINE lpLoadLibraryA = (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "LoadLibraryA");
		if (!lpLoadLibraryA) throw Error(ERRORDEF, Error::GenericWinApiFunctionError, { hProcess }, true);

		LPVOID lpAllocation = VirtualAllocEx(hProcess, nullptr, nStrLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!lpAllocation) throw Error(ERRORDEF, Error::VirtualAllocError, { hProcess }, true);

		if (!WriteProcessMemory(hProcess, lpAllocation, szRealPath, nStrLen, nullptr)) throw Error(ERRORDEF, Error::WinWriteError, { hProcess }, true);

		HANDLE hThread = CreateRemoteThread(hProcess, nullptr, NULL, lpLoadLibraryA, lpAllocation, NULL, nullptr);
		if (!hThread) throw Error(ERRORDEF, Error::GenericWinApiFunctionError, { hProcess }, true);
		CloseHandle(hThread);
		Sleep(10);

		if (!VirtualFreeEx(hProcess, lpAllocation, NULL, MEM_RELEASE)) throw Error(ERRORDEF, Error::VirtualFreeError, { hProcess }, true);

		CloseHandle(hProcess);

		return true;
	}
	CATCH_ERROR;
}

LIB_END