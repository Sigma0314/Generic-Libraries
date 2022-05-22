#ifndef LIB_MATHS_H
#define LIB_MATHS_H

#include <stdlib.h> // srand, rand
#include <time.h> // time

#include "EnvVar.h"
#include "Collections.h" // LiteralDynamicArray, RoundedDynamicArray

LIB_BEGIN

double Power(const double _Constant, const double _Exponent);

template<typename _Ty>
double Mean(RoundedDynamicArray<_Ty>& _Array) {
	_Ty sum = 0;
	size_t size = _Array.Size();
	for (size_t i = 0; i < size; i++) sum += _Array[i];
	return (double)sum / (double)size;
}

template<typename _Ty>
double Mean(LiteralDynamicArray<_Ty>& _Array) {
	_Ty sum = 0;
	size_t size = _Array.Size();
	for (size_t i = 0; i < size; i++) sum += _Array[i];
	return (double)sum / (double)size;
}

template<typename _Ty>
double Median(RoundedDynamicArray<_Ty>& _Array) {

}

template<typename _Ty>
double Median(LiteralDynamicArray<_Ty>& _Array) {

}

int Random(int _Low = 0, int _High = RAND_MAX);

LIB_END

#endif