#ifndef __MEM_SECURE_H__
#define __MEM_SECURE_H__

#include "def.h"
#include "error.h"

BOOL Read(ADDRESS uAddress, LPVOID lpReturn, DWORD dwSize, LPERROR lpError, BOOL bInvokeHandler);

#endif // !__MEM_SECURE_H__