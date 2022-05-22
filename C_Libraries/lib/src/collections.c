#include "../include/collections.h"
#include "../include/error.h"

BOOL APIENTRY AllocHeapArray(
	_Out_ LPHEAP_ARRAY lpArray,
	_In_ SIZE_T nSize
) {
	if (!lpArray) THROW(NULL_PARAM, FALSE);
	if (lpArray->bActive) THROW(ALREADY_REGISTERED, FALSE);

	lpArray->lpArray = malloc(nSize);
	lpArray->nSize = nSize;

	lpArray->bActive = TRUE;
	return SUCCEEDED;
}

BOOL APIENTRY Alloc2DHeapArray(
	_Out_ LPHEAP_2DARRAY lpArray,
	_In_ SIZE_T nSize1,
	_In_ SIZE_T nSize2
) {
	if (!lpArray) THROW(NULL_PARAM, FALSE);
	if (lpArray->bActive) THROW(ALREADY_REGISTERED, FALSE);

	lpArray->lpArray = (LPVOID*)malloc(nSize1 * PTRSIZE);
	for (SIZE_T i = 0; i < nSize1; i++)
		lpArray->lpArray[i] = (LPVOID)malloc(nSize2);

	lpArray->nSize1 = nSize1;
	lpArray->nSize2 = nSize2;

	lpArray->bActive = TRUE;
	return SUCCEEDED;
}

BOOL APIENTRY Alloc3DHeapArray(
	_Out_ LPHEAP_3DARRAY lpArray,
	_In_ SIZE_T nSize1,
	_In_ SIZE_T nSize2,
	_In_ SIZE_T nSize3
) {
	if (!lpArray) THROW(NULL_PARAM, FALSE);
	if (lpArray->bActive) THROW(ALREADY_REGISTERED, FALSE);

	lpArray->lpArray = (LPVOID**)malloc(nSize1 * PTRSIZE);
	for (SIZE_T i = 0; i < nSize1; i++) {
		lpArray->lpArray[i] = (LPVOID*)malloc(nSize2 * PTRSIZE);
		for (SIZE_T j = 0; j < nSize2; j++) lpArray->lpArray[j] = (LPVOID)malloc(nSize3);
	}

	lpArray->nSize1 = nSize1;
	lpArray->nSize2 = nSize2;
	lpArray->nSize3 = nSize3;

	lpArray->bActive = TRUE;
	return SUCCEEDED;
}

BOOL APIENTRY AllocRDA(
	_Out_ LPROUNDED_DYNAMIC_ARRAY lpArray
) {
	if (!lpArray) THROW(NULL_PARAM, FALSE);
	if (lpArray->bActive) THROW(ALREADY_REGISTERED, FALSE);

	lpArray->lpArray = malloc(RDA_PAGE_SIZE);
	lpArray->nSize = NULL;
	lpArray->nAllocSize = RDA_PAGE_SIZE;
	lpArray->nPageSize = RDA_PAGE_SIZE;

	lpArray->bActive = TRUE;
	return SUCCEEDED;
}

BOOL APIENTRY AllocLDA(
	_Out_ LPLITERAL_DYNAMIC_ARRAY lpArray
) {
	if (!lpArray) THROW(NULL_PARAM, FALSE);
	if (lpArray->bActive) THROW(ALREADY_REGISTERED, FALSE);

	lpArray->lpArray = NULL;
	lpArray->nSize = NULL;

	lpArray->bActive = TRUE;
	return SUCCEEDED;
}

BOOL APIENTRY DeleteHeapArray(
	_In_ LPHEAP_ARRAY lpArray
) {
	if (!lpArray) THROW(NULL_PARAM, FALSE);
	if (!lpArray->bActive) THROW(ALREADY_REGISTERED, FALSE);

	free(lpArray->lpArray);

	lpArray->bActive = FALSE;
	return SUCCEEDED;
}

BOOL APIENTRY Delete2DHeapArray(
	_In_ LPHEAP_2DARRAY lpArray
) {
	if (!lpArray) THROW(NULL_PARAM, FALSE);
	if (!lpArray->bActive) THROW(ALREADY_REGISTERED, FALSE);

	for (SIZE_T i = 0; i < lpArray->nSize1; i++) free(lpArray->lpArray[i]);
	free(lpArray->lpArray);

	lpArray->bActive = FALSE;
	return SUCCEEDED;
}

BOOL APIENTRY Delete3DHeapArray(
	_In_ LPHEAP_3DARRAY lpArray
) {
	if (!lpArray) THROW(NULL_PARAM, FALSE);
	if (!lpArray->bActive) THROW(ALREADY_REGISTERED, FALSE);

	for (SIZE_T i = 0; i < lpArray->nSize1; i++) {
		for (SIZE_T j = 0; j < lpArray->nSize2; i++) free(lpArray->lpArray[i][j]);
		free(lpArray->lpArray[i]);
	}
	free(lpArray->lpArray);

	lpArray->bActive = FALSE;
	return SUCCEEDED;
}

BOOL APIENTRY DeleteRDA(
	_In_ LPROUNDED_DYNAMIC_ARRAY lpArray
) {
	if (!lpArray) THROW(NULL_PARAM, FALSE);
	if (!lpArray->bActive) THROW(ALREADY_REGISTERED, FALSE);

	free(lpArray->lpArray);

	lpArray->bActive = FALSE;
	return SUCCEEDED;
}

BOOL APIENTRY DeleteLDA(
	_In_ LPLITERAL_DYNAMIC_ARRAY lpArray
) {
	if (!lpArray) THROW(NULL_PARAM, FALSE);
	if (!lpArray->bActive) THROW(ALREADY_REGISTERED, FALSE);

	free(lpArray->lpArray);

	lpArray->bActive = FALSE;
	return SUCCEEDED;
}

#define RDA_REALLOC_SIZE(nSize) (SIZE_T)((nSize % RDA_PAGE_SIZE) ? (nSize + RDA_PAGE_SIZE) : nSize)

BOOL APIENTRY ReallocRDA(LPROUNDED_DYNAMIC_ARRAY lpArray, SIZE_T nNewSize) {
	if (!lpArray) THROW(NULL_PARAM, FALSE);

	LPVOID lpOld = lpArray->lpArray;
	SIZE_T nOldSize = lpArray->nAllocSize;

	lpArray->nAllocSize = RDA_REALLOC_SIZE(nNewSize);
	lpArray->lpArray = (LPVOID)malloc(lpArray->nAllocSize);

	if (lpOld) {
		memcpy_s(lpArray->lpArray, lpArray->nAllocSize, lpOld, nOldSize);
		free(lpOld);
	}

	return SUCCEEDED;
}

BOOL APIENTRY ReallocLDA(LPLITERAL_DYNAMIC_ARRAY lpArray, SIZE_T nNewSize) {
	if (!lpArray) THROW(NULL_PARAM, FALSE);

	LPVOID lpOld = lpArray->lpArray;
	SIZE_T nOldSize = lpArray->nSize;

	lpArray->lpArray = malloc(nNewSize);
	lpArray->nSize = nNewSize;

	if (lpOld) {
		SIZE_T nCpy = nOldSize > nNewSize ? nNewSize : nOldSize;
		memcpy_s(lpArray->lpArray, lpArray->nSize, lpOld, nCpy);
		free(lpOld);
	}

	return SUCCEEDED;
}

BOOL APIENTRY PushBackRDA(
	_Inout_ LPROUNDED_DYNAMIC_ARRAY lpArray,
	_In_ LPVOID lpData,
	_In_ DWORD dwSize
) {
	if (!lpArray || !lpData) THROW(NULL_PARAM, FALSE);
	if (!lpArray->bActive) THROW(INVALIDPARAM, FALSE);

	if (lpArray->nSize + dwSize > lpArray->nAllocSize) ReallocRDA(lpArray, lpArray->nSize + dwSize);

	memcpy_s((LPVOID)((ADDRESS)lpArray->lpArray + lpArray->nSize), lpArray->nAllocSize - lpArray->nSize, lpData, dwSize);

	return SUCCEEDED;
}

BOOL APIENTRY PushBackLDA(
	_Inout_ LPLITERAL_DYNAMIC_ARRAY lpArray,
	_In_ LPVOID lpData,
	_In_ DWORD dwSize
) {
	if (!lpArray || !lpData) THROW(NULL_PARAM, FALSE);
	if (!lpArray->bActive) THROW(INVALIDPARAM, FALSE);

	SIZE_T nOldSize = lpArray->nSize;
	if (!ReallocLDA(lpArray, lpArray->nSize + dwSize)) return FALSE;

	memcpy_s((LPVOID)((ADDRESS)lpArray->lpArray + nOldSize), lpArray->nSize, lpData, dwSize);

	return SUCCEEDED;
}

BOOL APIENTRY PopBackRDA(
	_Inout_ LPROUNDED_DYNAMIC_ARRAY lpArray,
	_In_ SIZE_T nCount
) {
	if (!lpArray || !nCount) THROW(NULL_PARAM, FALSE);
	if (!lpArray->bActive || !lpArray->nSize) THROW(INVALIDPARAM, FALSE);

	memset((LPVOID)((ADDRESS)lpArray->lpArray + lpArray->nSize - nCount), 0x00, nCount);

	return SUCCEEDED;
}

BOOL APIENTRY PopBackLDA(
	_Inout_ LPLITERAL_DYNAMIC_ARRAY lpArray,
	_In_ SIZE_T nCount
) {
	if (!lpArray || !nCount) THROW(NULL_PARAM, FALSE);
	if (!lpArray->bActive || !lpArray->nSize) THROW(INVALIDPARAM, FALSE);

	return ReallocLDA(lpArray, lpArray->nSize - nCount);
}