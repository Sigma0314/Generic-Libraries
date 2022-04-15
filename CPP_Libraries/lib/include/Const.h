#pragma once
#include <initializer_list> // std::initializer_list

#include "EnvVar.h"

LIB_BEGIN

template<typename _Ty>
class _InitListWrap {
	using _TyList = std::initializer_list<_Ty>;
	_TyList _List;

public:
	_InitListWrap(_In_ _TyList _List) : _List(_List) {}

	size_t Size() const { return _List.size(); }

	_Ty& operator[](size_t i) const { return *(_Ty*)(_List.begin() + i); }
};
using Handles = _InitListWrap<void*>;

LIB_END