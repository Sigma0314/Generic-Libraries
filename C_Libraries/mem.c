#include "mem.h"

BOOL GetProcessInfoA(LPPROCNFOA lppnfo) {
	if (!lppnfo) return FALSE;
	lppnfo->hProcess = GetCurrentProcess();
	lppnfo->hModule = GetModuleHandleA(NULL);
	lppnfo->dwProcId = GetCurrentProcessId();

	if (!K32GetModuleFileNameExA(lppnfo->hProcess, lppnfo->hModule, lppnfo->szProcName, MAX_PATH)) return FALSE;

	MODULEINFO mnfo;
	if (!K32GetModuleInformation(lppnfo->hProcess, lppnfo->hModule, &mnfo, sizeof(mnfo))) return FALSE;

	lppnfo->uModBase = (ADDRESS)lppnfo->hModule;
	lppnfo->dwModSize = mnfo.SizeOfImage;

	return TRUE;
}

BOOL GetProcessInfoW(LPPROCNFOW lppnfo) {
	if (!lppnfo) return FALSE;
	lppnfo->hProcess = GetCurrentProcess();
	lppnfo->hModule = GetModuleHandleW(NULL);
	lppnfo->dwProcId = GetCurrentProcessId();

	if (!K32GetModuleFileNameExW(lppnfo->hProcess, lppnfo->hModule, lppnfo->szProcName, MAX_PATH)) return FALSE;

	MODULEINFO mnfo;
	if (!K32GetModuleInformation(lppnfo->hProcess, lppnfo->hModule, &mnfo, sizeof(mnfo))) return FALSE;

	lppnfo->uModBase = (ADDRESS)lppnfo->hModule;
	lppnfo->dwModSize = mnfo.SizeOfImage;

	return TRUE;
}

BOOL GetModuleInfoA(LPMODNFOA lpmnfo, LPCSTR lpModName) {
	if (!lpmnfo) return FALSE;
	lpmnfo->hModule = GetModuleHandleA(lpModName);

	if (!K32GetModuleFileNameExA(GetCurrentProcess(), lpmnfo->hModule, lpmnfo->szModName, MAX_PATH)) return FALSE;

	MODULEINFO mnfo;
	if (!K32GetModuleInformation(GetCurrentProcess(), lpmnfo->hModule, &mnfo, sizeof(mnfo))) return FALSE;

	lpmnfo->uModBase = (ADDRESS)lpmnfo->hModule;
	lpmnfo->dwModSize = mnfo.SizeOfImage;

	return TRUE;
}

BOOL GetModuleInfoW(LPMODNFOW lpmnfo, LPCWSTR lpModName) {
	if (!lpmnfo) return FALSE;
	lpmnfo->hModule = GetModuleHandleW(lpModName);

	if (!K32GetModuleFileNameExW(GetCurrentProcess(), lpmnfo->hModule, lpmnfo->szModName, MAX_PATH)) return FALSE;

	MODULEINFO mnfo;
	if (!K32GetModuleInformation(GetCurrentProcess(), lpmnfo->hModule, &mnfo, sizeof(mnfo))) return FALSE;

	lpmnfo->uModBase = (ADDRESS)lpmnfo->hModule;
	lpmnfo->dwModSize = mnfo.SizeOfImage;

	return TRUE;
}

BOOL CheckPtr(ADDRESS uAddress, DWORD dwSize) {
	__try {
		for (INT i = 0; i < dwSize; i++) Read(BYTE, uAddress + i);
		return TRUE;
	}
	__except (1) {
		return FALSE;
	}
}