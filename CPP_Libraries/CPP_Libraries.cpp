#include <stdio.h>

#include <Windows.h>

struct NtCreateThreadExBuffer {
	ULONG Size;
	ULONG Unknown1;
	ULONG Unknown2;
	PULONG Unknown3;
	ULONG Unknown4;
	ULONG Unknown5;
	ULONG Unknown6;
	PULONG Unknown7;
	ULONG Unknown8;
};

typedef NTSTATUS(WINAPI* tNtCreateThreadEx)(
	OUT PHANDLE hThread,
	IN ACCESS_MASK DesiredAccess,
	IN LPVOID ObjectAttributes,
	IN HANDLE ProcessHandle,
	IN LPTHREAD_START_ROUTINE lpStartAddress,
	IN LPVOID lpParameter,
	IN BOOL CreateSuspended,
	IN ULONG StackZeroBits,
	IN ULONG SizeOfStackCommit,
	IN ULONG SizeOfStackReserve,
	OUT LPVOID lpBytesBuffer
);

void TestCall() {
	HANDLE hProcess = nullptr;
	LPVOID lpAlloc = nullptr;

	try {
		DWORD dwProcessId = 4256;
		const char* lpDllPath = "C:\\Users\\dougl\\Desktop\\TestDll64.dll";

		HMODULE hNtDll = GetModuleHandleA("ntdll.dll");
		if (!hNtDll) throw 1;

		tNtCreateThreadEx NtCreateThreadEx = (tNtCreateThreadEx)GetProcAddress(hNtDll, "NtCreateThreadEx");
		if (!NtCreateThreadEx) throw 2;

		NtCreateThreadExBuffer ntBuffer;
		memset(&ntBuffer, 0x00, sizeof(ntBuffer));

		DWORD dwT1 = NULL, dwT2 = NULL;

		ntBuffer.Size = sizeof(NtCreateThreadExBuffer);
		ntBuffer.Unknown1 = 0x10003;
		ntBuffer.Unknown2 = 0x8;
		ntBuffer.Unknown3 = &dwT2;
		ntBuffer.Unknown4 = 0;
		ntBuffer.Unknown5 = 0x10004;
		ntBuffer.Unknown6 = 4;
		ntBuffer.Unknown7 = &dwT1;
		ntBuffer.Unknown8 = 0;

		hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, dwProcessId);
		if (!hProcess) throw 3;

		lpAlloc = VirtualAllocEx(hProcess, nullptr, strlen(lpDllPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!lpAlloc) throw 4;

		if (!WriteProcessMemory(hProcess, lpAlloc, lpDllPath, strlen(lpDllPath) + 1, nullptr)) throw 5;

		HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
		if (!hKernel32) throw 6;

		LPTHREAD_START_ROUTINE lpLoadLibraryA = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryA");
		if (!lpLoadLibraryA) throw 7;

		HANDLE hThread = nullptr;
		NTSTATUS ntStatus = NtCreateThreadEx(&hThread, 0x1FFFFF, nullptr, hProcess, lpLoadLibraryA, lpAlloc, FALSE, NULL, NULL, NULL, &ntBuffer);
		if (!hThread) { printf("0x%lX\n", ntStatus); throw 8; }

		WaitForSingleObject(hThread, INFINITE);

		DWORD dwLoadLibraryAExitCode;
		GetExitCodeThread(hThread, &dwLoadLibraryAExitCode);

		CloseHandle(hThread);

		printf("LoadLibraryA exit code: %lu\n", dwLoadLibraryAExitCode);

		if (!dwLoadLibraryAExitCode) {
			LPTHREAD_START_ROUTINE lpGetLastError = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "GetLastError");
			if (!lpGetLastError) throw 9;

			hThread = CreateRemoteThread(hProcess, nullptr, NULL, lpGetLastError, nullptr, NULL, nullptr);
			if (!hThread) throw 10;

			WaitForSingleObject(hThread, INFINITE);

			DWORD dwGetLastErrorExitCode;
			GetExitCodeThread(hThread, &dwGetLastErrorExitCode);

			CloseHandle(hThread);

			printf("LoadLibraryA error code: %lu\n", dwGetLastErrorExitCode);
		}

		if (!VirtualFreeEx(hProcess, lpAlloc, NULL, MEM_RELEASE)) throw 11;

		if (!CloseHandle(hProcess)) throw 12;
	}
	catch (int n) {
		printf("%i\n", n);

		switch (n) {
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:

			if (hProcess && lpAlloc) VirtualFreeEx(hProcess, lpAlloc, NULL, MEM_RELEASE);
		case 4:
		case 11:

			if (hProcess) CloseHandle(hProcess);
		default:
			break;
		}
	}
}

int main() {

	return NULL;
}