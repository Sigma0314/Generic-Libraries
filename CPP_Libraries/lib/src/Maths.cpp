#include "../include/Maths.h"

LIB_BEGIN

double Power(const double _Constant, const double _Exponent) {
	if (_Exponent > 0) {
		double ret = _Constant;
		for (int i = 1; i < _Exponent; i++) ret *= _Constant;
		return ret;
	}

	else if (_Exponent < 0) return 1 / Power(_Constant, -_Exponent);

	else return 1;
}

int Random(int _Low, int _High) {
	int ret = NULL;

	do {
		srand(time(NULL));
		ret = rand();
		if (ret) ret %= _High + 1;
	} while (!IN_RANGE(ret, _Low, _High));

	return ret;
}

LIB_END