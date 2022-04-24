#ifndef LIB_CONST_H
#define LIB_CONST_H

#include <initializer_list> // std::initializer_list

#include "EnvVar.h"

LIB_BEGIN

template<typename _Ty>
class ArrayParam {
	using _TyList = std::initializer_list<_Ty>;
	_TyList _List;

public:
	ArrayParam(_In_ _TyList _List) : _List(_List) {}

	size_t Size() const { return _List.size(); }

	_Ty& operator[](size_t i) const { return *(_Ty*)(_List.begin() + i); }
};
typedef ArrayParam<void*> Handles;

LIB_END

#endif // !LIB_CONST_H