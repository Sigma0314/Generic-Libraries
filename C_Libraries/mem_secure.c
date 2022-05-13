#include "mem_secure.h"

BOOL Read(ADDRESS uAddress, LPVOID lpReturn, DWORD dwSize, LPERROR lpError, BOOL bInvokeHandler) {
	__try {
		for (INT i = 0; i < dwSize; i++) {
			*(BYTE*)((ADDRESS)lpReturn + i) = *(BYTE*)(uAddress + i);
		}
		
		return TRUE;
	} 
	__except (1) { ReturnError(FALSE, lpReturn, 0x01, TEXT("Dereferenced invalid memory."), FALSE, DEBUG_PARAM, bInvokeHandler); }
}