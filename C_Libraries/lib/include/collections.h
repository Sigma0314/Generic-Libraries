#ifndef __COLLECTIONS_H__
#define __COLLECTIONS_H__

#include "envvar.h"
#include "def.h"

typedef struct _PAIR {
	LPVOID lp1, lp2;
} PAIR, * LPPAIR;

typedef struct _VEC2 {
	float x, y;
} VEC2, * LPVEC2;

typedef struct _VEC3 {
	float x, y, z;
} VEC3, * LPVEC3;

typedef struct _VEC4 {
	float x, y, z, w;
} VEC4, * LPVEC4;

typedef struct _HEAP_ARRAY {
	LPVOID lpArray;
	SIZE_T nSize;
	BOOL bActive;
} HEAP_ARRAY, * LPHEAP_ARRAY;

typedef struct _HEAP_2DARRAY {
	LPVOID* lpArray;
	SIZE_T nSize1, nSize2;
	BOOL bActive;
} HEAP_2DARRAY, * LPHEAP_2DARRAY;

typedef struct _HEAP_3DARRAY {
	LPVOID** lpArray;
	SIZE_T nSize1, nSize2, nSize3;
	BOOL bActive;
} HEAP_3DARRAY, * LPHEAP_3DARRAY;

typedef struct _ROUNDED_DYNAMIC_ARRAY {
	LPVOID lpArray;
	SIZE_T nSize;
	SIZE_T nAllocSize;
	SIZE_T nPageSize;
	BOOL bActive;
} ROUNDED_DYNAMIC_ARRAY, * LPROUNDED_DYNAMIC_ARRAY;

typedef struct _LITERAL_DYNAMIC_ARRAY {
	LPVOID lpArray;
	SIZE_T nSize;
	BOOL bActive;
} LITERAL_DYNAMIC_ARRAY, * LPLITERAL_DYNAMIC_ARRAY;

BOOL APIENTRY AllocHeapArray(
	_Out_ LPHEAP_ARRAY lpArray,
	_In_ SIZE_T nSize
);

BOOL APIENTRY Alloc2DHeapArray(
	_Out_ LPHEAP_2DARRAY lpArray,
	_In_ SIZE_T nSize1,
	_In_ SIZE_T nSize2
);

BOOL APIENTRY Alloc3DHeapArray(
	_Out_ LPHEAP_3DARRAY lpArray,
	_In_ SIZE_T nSize1,
	_In_ SIZE_T nSize2,
	_In_ SIZE_T nSize3
);

BOOL APIENTRY AllocRDA(
	_Out_ LPROUNDED_DYNAMIC_ARRAY lpArray
);

BOOL APIENTRY AllocLDA(
	_Out_ LPLITERAL_DYNAMIC_ARRAY lpArray
);

BOOL APIENTRY DeleteHeapArray(
	_In_ LPHEAP_ARRAY lpArray
);

BOOL APIENTRY Delete2DHeapArray(
	_In_ LPHEAP_2DARRAY lpArray
);

BOOL APIENTRY Delete3DHeapArray(
	_In_ LPHEAP_3DARRAY lpArray
);

BOOL APIENTRY DeleteRDA(
	_In_ LPROUNDED_DYNAMIC_ARRAY lpArray
);

BOOL APIENTRY DeleteLDA(
	_In_ LPLITERAL_DYNAMIC_ARRAY lpArray
);

BOOL APIENTRY PushBackRDA(
	_Inout_ LPROUNDED_DYNAMIC_ARRAY lpArray,
	_In_ LPVOID lpData,
	_In_ DWORD dwSize
);

BOOL APIENTRY PushBackLDA(
	_Inout_ LPLITERAL_DYNAMIC_ARRAY lpArray,
	_In_ LPVOID lpData,
	_In_ DWORD dwSize
);

BOOL APIENTRY PopBackRDA(
	_Inout_ LPROUNDED_DYNAMIC_ARRAY lpArray,
	_In_ SIZE_T nCount
);

BOOL APIENTRY PopBackLDA(
	_Inout_ LPLITERAL_DYNAMIC_ARRAY lpArray,
	_In_ SIZE_T nCount
);

#endif // !__COLLECTIONS_H__