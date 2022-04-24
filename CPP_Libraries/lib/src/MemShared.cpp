#include "../include/MemShared.h"

LIB_BEGIN

bool CheckVecMask(_In_ const char* _Mask, _In_ size_t _Count) {
	bool* v = new bool[_Count];
	memset(v, 0x00, _Count);

	char n;

	try {
		if (strlen(_Mask) != _Count) throw NULL;

		for (size_t i = 0; i < _Count; i++) {
			n = _Mask[i] - 0x30;
			if (n > _Count || n < 0) throw NULL;
			if (v[n - 1]) throw NULL;
			v[n - 1] = true;
		}

		delete[] v;
		return true;
	}
	catch (...) { delete[] v; return false; }
}

void substr(char* _Buffer, size_t _SizeOfBuffer, const char* _String, size_t _Index, size_t _Count = -1) {
	if (_Count == -1) _Count = strlen(_String) - _Index;

	strncpy_s(_Buffer, _SizeOfBuffer, &_String[_Index], _Count);
	_Buffer[_Count] = '\0';
}

#define HEXC_TO_UINT(c) (IN_RANGE(c, 'A', 'F') ? c - '\x37' : (IN_RANGE(c, 'a', 'f') ? c - '\x57' : c - '\x30'));

bool ParsePattern(
	_In_ const char* _ComboPattern,
	_Out_writes_all_(_PatternSize) char* _Pattern,
	_In_ size_t _PatternSize,
	_Out_writes_all_(_MaskSize) char* _Mask,
	_In_ size_t _MaskSize,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		if (!_Pattern || !_Mask) throw Error(ERRORDEF, Error::ReturnIsNullError);

		int vIndex[MAX_PATTERN_LENGTH];
		size_t vIndexI = 0;

		size_t nStrLen = strlen(_ComboPattern);
		for (size_t i = 0; i < nStrLen; i++) {
			if (_ComboPattern[i] == ' ') {
				vIndex[vIndexI] = i;
				vIndexI++;
			}
		}

		char vBytes[MAX_PATTERN_LENGTH][3];
		int vBytesI = 0;

		if (vIndexI) {
			int nLastIndex = vIndex[0];

			substr(vBytes[0], sizeof(vBytes[0]), _ComboPattern, 0, vIndex[0]);
			vBytesI++;
			for (int i = 1; i < vIndexI; i++) {
				substr(vBytes[i], sizeof(vBytes[i]), _ComboPattern, nLastIndex + 1, vIndex[i] - nLastIndex - 1);
				vBytesI++;
				nLastIndex = vIndex[i];
			}
			substr(vBytes[vBytesI], sizeof(vBytes[vBytesI]), _ComboPattern, nLastIndex + 1);
			vBytesI++;

			if (vBytesI > _PatternSize || vBytesI > _MaskSize) throw Error(ERRORDEF, Error::BufferTooSmallError);

			Byte b;

			for (int i = 0; i < vBytesI; i++) {
				if (!strcmp(vBytes[i], "??") || (vBytes[i][0] == '?' && vBytes[i][1] == '\0')) {
					_Pattern[i] = '\xCC';
					_Mask[i] = '?';
					continue;
				}

				if (vBytes[i][0] == '?' && vBytes[i][1] != '\0') {
					_Pattern[i] = HEXC_TO_UINT(vBytes[i][1]);
					_Mask[i] = 'l';
					continue;
				}

				if (vBytes[i][1] == '?' && vBytes[i][0] != '\0') {
					_Pattern[i] = HEXC_TO_UINT(vBytes[i][0]);
					_Pattern[i] *= 0x10;
					_Mask[i] = 'h';
					continue;
				}

				sscanf_s(vBytes[i], "%hhx", &b);
				_Pattern[i] = b;
				_Mask[i] = 'x';
			}

			_Pattern[vBytesI] = '\0';
			_Mask[vBytesI] = '\0';
		}
		else throw Error(ERRORDEF, Error::ParameterInvalidError);

		return true;
	}
	CATCH_ERROR;
}

bool IsProtectionValid(_In_ ScanProtection _Protection) {
	switch (_Protection) {
	case ScanProtection::Exectute_ReadWrite:
	case ScanProtection::Execute_Read:
	case ScanProtection::Execute_Readonly:
	case ScanProtection::NoExecute_ReadWrite:
	case ScanProtection::NoExecute_Read:
	case ScanProtection::NoExecute_Readonly:
	case ScanProtection::ReadWrite:
	case ScanProtection::Read:
	case ScanProtection::Readonly:
		return true;
	default:
		return false;
	}
}

bool ResolveProtection(_In_ ScanProtection _DesiredProtection, _In_ DWORD _CurrentProtection) {
	switch (_DesiredProtection) {
	case ScanProtection::Exectute_ReadWrite:
		if (_CurrentProtection == PAGE_EXECUTE_READWRITE) return true;
		else return false;
	case ScanProtection::Execute_Readonly:
		if (_CurrentProtection == PAGE_EXECUTE_READ) return true;
		else return false;
	case ScanProtection::Execute_Read:
		if (_CurrentProtection == PAGE_EXECUTE_READ || _CurrentProtection == PAGE_EXECUTE_READWRITE) return true;
		else return false;
	case ScanProtection::NoExecute_ReadWrite:
		if (_CurrentProtection == PAGE_READWRITE) return true;
		else return false;
	case ScanProtection::NoExecute_Readonly:
		if (_CurrentProtection == PAGE_READONLY) return true;
		else return false;
	case ScanProtection::NoExecute_Read:
		if (_CurrentProtection == PAGE_READONLY || _CurrentProtection == PAGE_READWRITE) return true;
		else return false;
	case ScanProtection::ReadWrite:
		if (_CurrentProtection == PAGE_EXECUTE_READWRITE || _CurrentProtection == PAGE_READWRITE) return true;
		else return false;
	case ScanProtection::Readonly:
		if (_CurrentProtection == PAGE_EXECUTE_READ || _CurrentProtection == PAGE_READONLY) return true;
		else return false;
	case ScanProtection::Read:
		if (_CurrentProtection == PAGE_EXECUTE_READWRITE ||
			_CurrentProtection == PAGE_READWRITE ||
			_CurrentProtection == PAGE_EXECUTE_READ ||
			_CurrentProtection == PAGE_READONLY) return true;
		else return false;
	default:
		return false;
	}
}

LIB_END