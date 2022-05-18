#ifndef LIB_COLLECTIONS_H
#define LIB_COLLECTIONS_H

#include <string.h> // memcpy_s, memcpy
#include <Windows.h> // RaiseException, EXCEPTION_ARRAY_BOUNDS_EXCEEDED

#include <initializer_list> // std::initializer_list

#include "EnvVar.h"

LIB_BEGIN

template<typename _Ty1, typename _Ty2>
class Pair {
public:
	_Ty1 _Obj1;
	_Ty2 _Obj2;

	Pair() : _Obj1((_Ty1)NULL), _Obj2((_Ty2)NULL) {}

	Pair(_In_opt_ _Ty1 _Param1, _In_opt_ _Ty2 _Param2) : _Obj1(_Param1), _Obj2(_Param2) {}
};

template<typename _Ty = float>
class Vec2 {
public:
	_Ty _Vec[2];

	Vec2() : _Vec() {}

	Vec2(_In_ _Ty n) {
		this->_Vec[0] = n;
		this->_Vec[1] = n;
	}

	Vec2(_In_ _Ty x, _In_ _Ty y) {
		this->_Vec[0] = x;
		this->_Vec[1] = y;
	}

	_Ty& X() { return this->_Vec[0]; }
	_Ty& Y() { return this->_Vec[1]; }

	_Ty& operator[](size_t i) { return this->_Vec[i]; }
};

template<typename _Ty = float>
class Vec3 {
public:
	_Ty _Vec[3];

	Vec3() : _Vec() {}

	Vec3(_In_ _Ty n) {
		this->_Vec[0] = n;
		this->_Vec[1] = n;
		this->_Vec[2] = n;
	}

	Vec3(_In_ _Ty x, _In_ _Ty y, _In_ _Ty z) {
		this->_Vec[0] = x;
		this->_Vec[1] = y;
		this->_Vec[2] = z;
	}

	Vec3(_In_ Vec2<_Ty> _Vec2, _In_ _Ty z) {
		this->_Vec[0] = _Vec2[0];
		this->_Vec[1] = _Vec2[1];
		this->_Vec[2] = z;
	}

	_Ty& X() { return this->_Vec[0]; }
	_Ty& R() { return this->_Vec[0]; }

	_Ty& Y() { return this->_Vec[1]; }
	_Ty& G() { return this->_Vec[1]; }

	_Ty& Z() { return this->_Vec[2]; }
	_Ty& B() { return this->_Vec[2]; }

	_Ty& operator[](size_t i) { return this->_Vec[i]; }
};

template<typename _Ty = float>
class Vec4 {
public:
	_Ty _Vec[4];

	Vec4() : _Vec() {}

	Vec4(_In_ _Ty n) {
		this->_Vec[0] = n;
		this->_Vec[1] = n;
		this->_Vec[2] = n;
		this->_Vec[3] = n;
	}

	Vec4(_In_ _Ty x, _In_ _Ty y, _In_ _Ty z, _In_ _Ty w) {
		this->_Vec[0] = x;
		this->_Vec[1] = y;
		this->_Vec[2] = z;
		this->_Vec[3] = w;
	}

	Vec4(_In_ Vec2<_Ty> _Vec2, _In_ _Ty z, _In_ _Ty w) {
		this->_Vec[0] = _Vec2[0];
		this->_Vec[1] = _Vec2[1];
		this->_Vec[2] = z;
		this->_Vec[3] = w;
	}

	Vec4(_In_ Vec3<_Ty> _Vec3, _In_ _Ty w) {
		this->_Vec[0] = _Vec3[0];
		this->_Vec[1] = _Vec3[1];
		this->_Vec[2] = _Vec3[2];
		this->_Vec[3] = w;
	}

	_Ty& X() { return this->_Vec[0]; }
	_Ty& R() { return this->_Vec[0]; }

	_Ty& Y() { return this->_Vec[1]; }
	_Ty& G() { return this->_Vec[1]; }

	_Ty& Z() { return this->_Vec[2]; }
	_Ty& B() { return this->_Vec[2]; }

	_Ty& W() { return this->_Vec[3]; }
	_Ty& A() { return this->_Vec[3]; }

	_Ty& operator[](size_t i) { return this->_Vec[i]; }
};

template<typename _Ty>
class Heap2DArray {
	size_t _Size1, _Size2;
	_Ty** _Array;
	bool _Allocated;

public:
	_Ty*& operator[](size_t i) { return this->_Array[i]; }

	Heap2DArray() : _Size1(NULL), _Size2(NULL), _Array(nullptr), _Allocated(false) {}

	Heap2DArray(_In_ size_t _Size1, _In_ size_t _Size2) : _Size1(_Size1), _Size2(_Size2), _Allocated(true) {
		this->_Array = new _Ty * [_Size1];
		for (size_t i = 0; i < _Size1; i++) this->_Array[i] = new _Ty[_Size2];
	}

	void Delete() {
		if (this->_Allocated) {
			for (size_t i = 0; i < _Size1; i++) delete[] this->_Array[i];
			delete[] this->_Array;
			this->_Allocated = false;
		}
	}

	~Heap2DArray() { this->Delete(); }

	bool IsAllocated() const { return this->_Allocated; }
};

template<typename _Ty>
class Heap3DArray {
	size_t _Size1, _Size2, _Size3;
	_Ty*** _Array;
	bool _Allocated;

public:
	_Ty**& operator[](size_t nIndex) { return this->_Array[nIndex]; }

	Heap3DArray() : _Size1(NULL), _Size2(NULL), _Size3(NULL), _Array(nullptr), _Allocated(false) {}

	Heap3DArray(_In_ size_t _Size1, _In_ size_t _Size2, _In_ size_t _Size3) : _Size1(_Size1), _Size2(_Size2), _Size3(_Size3), _Allocated(true) {
		this->_Array = new _Ty * *[_Size1];
		for (size_t i = 0; i < _Size1; i++) {
			this->_Array[i] = new _Ty * [_Size2];
			for (size_t j = 0; j < _Size2; j++) this->_Array[i][j] = new _Ty[_Size3];
		}
	}

	void Delete() {
		if (this->_Allocated) {
			for (size_t i = 0; i < _Size1; i++) {
				for (size_t j = 0; j < _Size2; j++) delete[] this->_Array[i][j];
				delete[] this->_Array[i];
			}
			delete[] this->_Array;
			this->_Allocated = false;
		}
	}

	~Heap3DArray() { this->Delete(); }

	bool IsAllocated() const { return this->_Allocated; }
};

template<typename _Ty>
class RoundedDynamicArray {
	size_t _Size;
	size_t _AllocSize;
	_Ty* _Alloc;
	static const size_t c_PageSize = 128;

	using _ListTy = std::initializer_list<_Ty>;

	static size_t RoundedArraySize(size_t _Size) {
		size_t byteSize = _Size * sizeof(_Ty);
		// size_t cnt = byteSize / c_PageSize;

		if (byteSize % c_PageSize) return (byteSize + c_PageSize) / sizeof(_Ty);
		else return (byteSize) / sizeof(_Ty);
	}

	void Realloc(size_t _NewSize) {
		_Ty* _OldAlloc = this->_Alloc;
		size_t _OldAllocSize = this->_AllocSize;

		this->_AllocSize = this->RoundedArraySize(_NewSize);
		this->_Alloc = new _Ty[this->_AllocSize];

		if (_OldAlloc) {
			memcpy_s(this->_Alloc, this->_AllocSize * sizeof(_Ty), _OldAlloc, _OldAllocSize * sizeof(_Ty));
			delete[] _OldAlloc;
		}
	}

public:

	RoundedDynamicArray() : _Size(NULL), _AllocSize(NULL), _Alloc(nullptr) {}

	RoundedDynamicArray(_ListTy _Array) {
		this->_Size = _Array.size();
		this->_AllocSize = this->RoundedArraySize(_Array.size());
		this->_Alloc = new _Ty[this->_AllocSize];

		memcpy_s(this->_Alloc, this->_AllocSize * sizeof(_Ty), _Array.begin(), this->_Size * sizeof(_Ty));
	}

	~RoundedDynamicArray() { delete[] this->_Alloc; }

	void PushBack(_Ty _Elem) {
		if (this->_Size + 1 > this->_AllocSize) Realloc(this->_Size + 1);
		this->_Alloc[this->_Size] = _Elem;
		this->_Size++;
	}

	void PushBack(_ListTy _Array) {
		if (this->_Size + _Array.size() > this->_AllocSize) Realloc(this->_Size + _Array.size());
		memcpy(this->_Alloc + this->_Size * sizeof(_Ty), _Array.begin(), _Array.size() * sizeof(_Ty));
		this->_Size += _Array.size();
	}

	void PopBack() {
		if (!this->_Size) return;
		this->_Alloc[this->_Size - 1] = NULL;
		this->_Size--;
	}

	void PopBack(size_t _Count) {
		if (!this->_Size) return;
		memset(this->_Alloc + (this->_Size - _Count) * sizeof(_Ty), NULL, _Count);
		this->_Size -= _Count;
	}

	size_t Size() const { return this->_Size; }

	_Ty& operator[](size_t i) { if (i >= this->_Size) RaiseException(EXCEPTION_ARRAY_BOUNDS_EXCEEDED, NULL, NULL, nullptr); return this->_Alloc[i]; }
};

template<typename _Ty>
class LiteralDynamicArray {
	size_t _Size;
	_Ty* _Alloc;

	using _ListTy = std::initializer_list<_Ty>;

	void Realloc(size_t _Size) {
		_Ty* _OldAlloc = this->_Alloc;
		size_t _OldSize = this->_Size;

		this->_Alloc = new _Ty[_Size];
		this->_Size = _Size;

		if (_OldAlloc) {
			size_t _CpySize;
			if (_OldSize > this->_Size) _CpySize = this->_Size;
			else _CpySize = _OldSize;
			memcpy(this->_Alloc, _OldAlloc, _CpySize * sizeof(_Ty));
			delete[] _OldAlloc;
		}
	}

public:

	LiteralDynamicArray() : _Alloc(nullptr), _Size(NULL) {}

	LiteralDynamicArray(_ListTy _Array) {
		this->_Size = _Array.size();
		this->_Alloc = new _Ty[this->_Size];

		memcpy_s(this->_Alloc, this->_Size * sizeof(_Ty), _Array.begin(), _Array.size() * sizeof(_Ty));
	}

	~LiteralDynamicArray() { delete[] this->_Alloc; }

	void PushBack(_Ty _Elem) {
		Realloc(this->_Size + 1);

		this->_Alloc[this->_Size] = _Elem;
	}

	void PushBack(_ListTy _Array) {
		size_t _OldSize = this->_Size;
		Realloc(this->_Size + _Array.size());

		memcpy(this->_Alloc + _OldSize, _Array.begin(), _Array.size() * sizeof(_Ty));
	}

	void PopBack() { Realloc(this->_Size - 1); }

	void PopBack(size_t _Count) { Realloc(this->_Size - _Count); }

	size_t Size() const { return this->_Size; }

	_Ty& operator[](size_t i) { return this->_Alloc[i]; }
};

LIB_END

#endif