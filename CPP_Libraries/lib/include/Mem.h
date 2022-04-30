#ifndef LIB_MEM_H
#define LIB_MEM_H

#include <Windows.h>
#include <Psapi.h>

#include "MinHook.h"
#include "EnvVar.h"
#include "Error.h"
#include "MemShared.h"
#include "Collections.h"

LIB_BEGIN

enum class Convention { Cdecl, FastCall, StdCall, ThisCall, VectorCall };

template<typename _TyR, typename... _TyP>
class Function {
	using _FnCdecl = _TyR(__cdecl*)(_TyP...);
	using _FnFastCall = _TyR(__fastcall*)(_TyP...);
	using _FnStdCall = _TyR(__stdcall*)(_TyP...);
	using _FnThisCall = _TyR(__thiscall*)(_TyP...);
	using _FnVectorCall = _TyR(__vectorcall*)(_TyP...);

	Address _FnAddress;
	Convention _Convention;

	Error* _GlobalError;
	bool _UponCtorSuppressHandler;

	bool _IsCallable;

	static DWORD _Call(Address _Address, Convention _CallingConvention, _TyR* _Return, _TyP... _Params) {
		__try {
			switch (_CallingConvention) {
			case Convention::Cdecl: *_Return = ((_FnCdecl)(_Address))(_Params...); break;
			case Convention::FastCall: *_Return = ((_FnFastCall)(_Address))(_Params...); break;
			case Convention::StdCall: *_Return = ((_FnStdCall)(_Address))(_Params...); break;
			case Convention::ThisCall: *_Return = ((_FnThisCall)(_Address))(_Params...); break;
			case Convention::VectorCall: *_Return = ((_FnVectorCall)(_Address))(_Params...); break;
			default: return 1;
			}

			return 0;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return 2; }
	}

public:

	Function() : _FnAddress(NULL), _Convention((Convention)NULL), _GlobalError(nullptr), _UponCtorSuppressHandler(false), _IsCallable(false) {}

	Function(_In_ Address _FunctionAddress, _In_opt_ Convention _CallingConvention = Convention::FastCall, _Out_opt_ Error* _Error = nullptr, _In_opt_ bool _SuppressHandler = false) :
		_FnAddress(_FunctionAddress), _Convention(_CallingConvention), _GlobalError(_Error), _UponCtorSuppressHandler(_SuppressHandler), _IsCallable(true) {}

	_TyR Call(_TyP... _Params) {
		if (!this->_IsCallable) return (_TyR)NULL;

		try {
			_TyR _Buffer;

			switch (this->_Call(this->_FnAddress, this->_Convention, &_Buffer, _Params...)) {
			case 0: return _Buffer;
			case 1: throw Error(this->_UponCtorSuppressHandler, __FUNCTION__, __FILE__, __LINE__, Error::ParameterInvalidError);
			case 2: throw Error(this->_UponCtorSuppressHandler, __FUNCTION__, __FILE__, __LINE__, Error::FunctionFailedError);
			default: throw Error(this->_UponCtorSuppressHandler, __FUNCTION__, __FILE__, __LINE__, Error::UnknownError);
			}
		}
		catch (Error& e) { if (this->_GlobalError) *this->_GlobalError = e; return (_TyR)NULL; }
	}

	_TyR operator()(_TyP... _Params) { if (this->_IsCallable) return this->Call(_Params...); else return (_TyR)NULL; }

	static bool Call(_In_ Address _Address, _In_ Convention _CallingConvention, _Out_opt_ Error* _Error, _In_ bool _SuppressHandler, _Out_opt_ _TyR* _Return, _In_opt_ _TyP... _Params) {
		try {
			_TyR _Buffer;
			switch (_Call(_Address, _CallingConvention, &_Buffer, _Params...)) {
			case 0: break;
			case 1: throw Error(ERRORDEF, Error::ParameterInvalidError);
			case 2: throw Error(ERRORDEF, Error::FunctionFailedError);
			default: throw Error(ERRORDEF, Error::UnknownError);
			}

			if (_Return) *_Return = _Buffer;
			return true;
		}
		catch (Error& e) { if (_Error) *_Error = e; return false; }
	}

	static _TyR Call(_In_ Address _Address, _In_ Convention _CallingConvention, _In_opt_ _TyP... _Params) {
		__try {
			switch (_CallingConvention) {
			case Convention::Cdecl: return ((_FnCdecl)(_Address))(_Params...);
			case Convention::FastCall: return ((_FnFastCall)(_Address))(_Params...);
			case Convention::StdCall: return ((_FnStdCall)(_Address))(_Params...);
			case Convention::ThisCall: return ((_FnThisCall)(_Address))(_Params...);
			case Convention::VectorCall: return ((_FnVectorCall)(_Address))(_Params...);
			default: return (_TyR)(NULL);
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return (_TyR)(NULL); }
	}
};

template<typename... _TyP>
class VoidFunction {
	using _FnCdecl = void(__cdecl*)(_TyP...);
	using _FnFastCall = void(__fastcall*)(_TyP...);
	using _FnStdCall = void(__stdcall*)(_TyP...);
	using _FnThisCall = void(__thiscall*)(_TyP...);
	using _FnVectorCall = void(__vectorcall*)(_TyP...);

	Address _FnAddress;
	Convention _Convention;

	Error* _GlobalError;
	bool _UponCtorSuppressHandler;

	bool _IsCallable;

	static DWORD _Call(Address _Address, Convention _CallingConvention, _TyP... _Params) {
		__try {
			switch (_CallingConvention) {
			case Convention::Cdecl: ((_FnCdecl)(_Address))(_Params...); break;
			case Convention::FastCall: ((_FnFastCall)(_Address))(_Params...); break;
			case Convention::StdCall: ((_FnStdCall)(_Address))(_Params...); break;
			case Convention::ThisCall: ((_FnThisCall)(_Address))(_Params...); break;
			case Convention::VectorCall: ((_FnVectorCall)(_Address))(_Params...); break;
			default: return 1;
			}

			return 0;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return 2; }
	}

public:

	VoidFunction() : _FnAddress(NULL), _Convention((Convention)NULL), _GlobalError(nullptr), _UponCtorSuppressHandler(false), _IsCallable(false) {}

	VoidFunction(_In_ Address _FunctionAddress, _In_opt_ Convention _CallingConvention = Convention::FastCall, _Out_opt_ Error* _Error = nullptr, _In_opt_ bool _SuppressHandler = false) :
		_FnAddress(_FunctionAddress), _Convention(_CallingConvention), _GlobalError(_Error), _UponCtorSuppressHandler(_SuppressHandler), _IsCallable(true) {}

	void Call(_TyP... _Params) {
		if (!this->_IsCallable) return;

		try {
			switch (this->_Call(this->_FnAddress, this->_Convention, _Params...)) {
			case 0: return;
			case 1: throw Error(this->_UponCtorSuppressHandler, __FUNCTION__, __FILE__, __LINE__, Error::ParameterInvalidError);
			case 2: throw Error(this->_UponCtorSuppressHandler, __FUNCTION__, __FILE__, __LINE__, Error::FunctionFailedError);
			default: throw Error(this->_UponCtorSuppressHandler, __FUNCTION__, __FILE__, __LINE__, Error::UnknownError);
			}
		}
		catch (Error& e) { if (this->_GlobalError) *this->_GlobalError = e; return; }
	}

	void operator()(_TyP... _Params) { if (this->_IsCallable) this->Call(_Params...); }

	static bool Call(_In_ Address _Address, _In_ Convention _CallingConvention, _Out_opt_ Error* _Error, _In_ bool _SuppressHandler, _In_opt_ _TyP... _Params) {
		try {
			switch (_Call(_Address, _CallingConvention, _Params...)) {
			case 0: break;
			case 1: throw Error(ERRORDEF, Error::ParameterInvalidError);
			case 2: throw Error(ERRORDEF, Error::FunctionFailedError);
			default: throw Error(ERRORDEF, Error::UnknownError);
			}

			return true;
		}
		catch (Error& e) { if (_Error) *_Error = e; return false; }
	}

	static void Call(_In_ Address _Address, _In_ Convention _CallingConvention, _In_opt_ _TyP... _Params) {
		__try {
			switch (_CallingConvention) {
			case Convention::Cdecl: ((_FnCdecl)(_Address))(_Params...);
			case Convention::FastCall: ((_FnFastCall)(_Address))(_Params...);
			case Convention::StdCall: ((_FnStdCall)(_Address))(_Params...);
			case Convention::ThisCall: ((_FnThisCall)(_Address))(_Params...);
			case Convention::VectorCall: ((_FnVectorCall)(_Address))(_Params...);
			default: return;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return; }
	}
};

struct ProcessInfo {
	DWORD dwProcessId;
	HANDLE hProcess;
	HMODULE hModule;
	Address nModuleBase;
	size_t nModuleSize;
};

struct ModuleInfo {
	HMODULE hModule;
	Address nModuleBase;
	size_t nModuleSize;
};

bool GetProcessInfo(
	_Out_ ProcessInfo* _ProcessInfo,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

bool GetModuleInfo(
	_In_ const wchar_t* _ModuleName,
	_Out_ ModuleInfo* _ModuleInfo,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

bool InitializeHook(
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

bool UninitializeHook(
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

template<typename _TyFn>
bool CreateHook(
	_In_ LPVOID _Target,
	_In_ LPVOID _Detour,
	_Out_opt_ _TyFn* _Original,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
) {
	try {
		MH_STATUS st = MH_CreateHook(_Target, _Detour, (LPVOID*)_Original);
		if (st != MH_OK)
			if (st != MH_ERROR_MEMORY_ALLOC && st != MH_ERROR_MEMORY_PROTECT) throw Error(ERRORDEF, (Error::Code)(st));
			else throw Error(ERRORDEF, (Error::Code)(st), true);
		return true;
	}
	CATCH_ERROR;
}

bool RemoveHook(
	_In_ LPVOID _Target,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

template<typename _TyFn>
bool CreateAndEnableHook(
	_In_ LPVOID _Target,
	_In_ LPVOID _Detour,
	_Out_opt_ _TyFn* _Original,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
) {
	try {
		MH_STATUS st = MH_CreateHook(_Target, _Detour, (LPVOID*)_Original);
		if (st != MH_OK)
			if (st != MH_ERROR_MEMORY_ALLOC && st != MH_ERROR_MEMORY_PROTECT) throw Error(ERRORDEF, (Error::Code)(st));
			else throw Error(ERRORDEF, (Error::Code)(st), true);

		st = MH_EnableHook(_Target);
		if (st != MH_OK)
			if (st != MH_ERROR_MEMORY_ALLOC && st != MH_ERROR_MEMORY_PROTECT) throw Error(ERRORDEF, (Error::Code)(st));
			else throw Error(ERRORDEF, (Error::Code)(st), true);

		return true;
	}
	CATCH_ERROR;
}

bool EnableHook(
	_In_ LPVOID _Target,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

bool DisableHook(
	_In_ LPVOID _Target,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

template<typename _Ty>
bool _Read(
	_In_ Address _Address,
	_Out_ _Ty* _Return
) {
	__try {
		*_Return = *(_Ty*)(_Address);
		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

template<typename _Ty>
bool Read(
	_In_ Address _Address,
	_Out_ _Ty* _Return,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
) {
	try {
		if (!_Return) throw Error(ERRORDEF, Error::ReturnIsNullError);

		if (!_Read<_Ty>(_Address, _Return)) throw Error(ERRORDEF, Error::ReadError);

		return true;
	}
	CATCH_ERROR;
}

template<typename _Ty>
bool Read_Unsafe(
	_In_ Address _Address,
	_Out_ _Ty* _Return,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
) {
	try {
		if (!_Return) throw Error(ERRORDEF, Error::ReturnIsNullError);

		*_Return = *(_Ty*)(_Address);
		return true;
	}
	CATCH_ERROR;
}

bool GetAddress(
	_In_ Address _StartAddress,
	_In_ Offsets _Offsets,
	_Out_ Address* _Return,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

template<typename _Ty>
bool ReadPtr(
	_In_ Address _StartAddress,
	_In_ Offsets _Offsets,
	_Out_ _Ty* _Return,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
) {
	try {
		if (!_Return) throw Error(ERRORDEF, Error::ReturnIsNullError);

		if (!GetAddress(_StartAddress, _Offsets, &_StartAddress, _Error, _SuppressHandler)) return false;

		return Read(_StartAddress, _Return, _Error, _SuppressHandler);
	}
	CATCH_ERROR;
}

template<typename _Ty>
bool ReadVec2(
	_In_ Address _Address,
	_Out_ Vec2<_Ty>* _Return,
	_In_ const char* _Mask = "12",
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
) {
	try {
		if (!_Return) throw Error(ERRORDEF, Error::ReturnIsNullError);
		if (!CheckVecMask(_Mask, 2)) throw Error(ERRORDEF, Error::ParameterInvalidError);

		_Ty ret[2];
		Byte vec[2];

		for (size_t i = 0; i < 2; i++) vec[i] = _Mask[i] - 0x30;

		for (size_t i = 0; i < 2; i++) if (!Read(_Address + (sizeof(_Ty) * (vec[i] - 1)), &ret[i], _Error, _SuppressHandler)) return false;

		*_Return = { ret[0], ret[1] };

		return true;
	}
	CATCH_ERROR;
}

template<typename _Ty>
bool ReadVec3(
	_In_ Address _Address,
	_Out_ Vec3<_Ty>* _Return,
	_In_ const char* _Mask = "123",
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
) {
	try {
		if (!_Return) throw Error(ERRORDEF, Error::ReturnIsNullError);
		if (!CheckVecMask(_Mask, 3)) throw Error(ERRORDEF, Error::ParameterInvalidError);

		_Ty ret[3];
		Byte vec[3];

		for (size_t i = 0; i < 3; i++) vec[i] = _Mask[i] - 0x30;

		for (size_t i = 0; i < 3; i++) if (!Read(_Address + (sizeof(_Ty) * (vec[i] - 1)), &ret[i], _Error, _SuppressHandler)) return false;

		*_Return = { ret[0], ret[1], ret[2] };

		return true;
	}
	CATCH_ERROR;
}

template<typename _Ty>
bool ReadVec4(
	_In_ Address _Address,
	_Out_ Vec4<_Ty>* _Return,
	_In_ const char* _Mask = "1234",
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
) {
	try {
		if (!_Return) throw Error(ERRORDEF, Error::ReturnIsNullError);
		if (!CheckVecMask(_Mask, 4)) throw Error(ERRORDEF, Error::ParameterInvalidError);

		_Ty ret[4];
		Byte vec[4];

		for (size_t i = 0; i < 4; i++) vec[i] = _Mask[i] - 0x30;

		for (size_t i = 0; i < 4; i++) if (!Read(_Address + (sizeof(_Ty) * (vec[i] - 1)), &ret[i], _Error, _SuppressHandler)) return false;

		*_Return = { ret[0], ret[1], ret[2], ret[3] };

		return true;
	}
	CATCH_ERROR;
}

bool ReadStringA(
	_In_ Address _Address,
	_In_ size_t _Length,
	_Out_writes_all_(_ArrayLength) char* _Buffer,
	_In_ size_t _ArrayLength,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

bool ReadStringW(
	_In_ Address _Address,
	_In_ size_t _Length,
	_Out_writes_all_(_ArrayLength) wchar_t* _Buffer,
	_In_ size_t _ArrayLength,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

bool ReadBytes(
	_In_ Address _Address,
	_In_ size_t _Length,
	_Out_writes_all_(_ArrayLength) Byte* _Buffer,
	_In_ size_t _ArrayLength,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

template<typename _Ty>
bool _Write(
	_In_ Address _Address,
	_In_ _Ty _Data
) {
	__try {
		*(_Ty*)(_Address) = _Data;
		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

template<typename _Ty>
bool Write(
	_In_ Address _Address,
	_In_ _Ty _Data,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
) {
	try {
		if (!_Write<_Ty>(_Address, _Data)) throw Error(ERRORDEF, Error::WriteError);
		return true;
	}
	CATCH_ERROR;
}

template<typename _Ty>
bool Write_Unsafe(
	_In_ Address _Address,
	_In_ _Ty _Data
) {
	*(_Ty*)(_Address) = _Data;
	return true;
}

template<typename _Ty>
bool WritePtr(
	_In_ Address _StartAddress,
	_In_ Offsets _Offsets,
	_In_ _Ty _Data,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
) {
	if (!GetAddress(_StartAddress, _Offsets, &_StartAddress, _Error, _SuppressHandler)) return false;
	return Write(_StartAddress, _Data, _Error, _SuppressHandler);
}

template<typename _Ty>
bool WriteVec2(
	_In_ Address _Address,
	_In_ Vec2<_Ty> _Data,
	_In_ const char* _Mask = "12",
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
) {
	try {
		if (!_Data || !CheckVecMask(_Mask, 2)) throw Error(ERRORDEF, Error::ParameterInvalidError);

		Byte vec[2];

		for (size_t i = 0; i < 2; i++) vec[i] = _Mask[i] - 0x30;

		for (size_t i = 0; i < 2; i++) if (!Write(_Address + (sizeof(_Ty) * (vec[i] - 1)), _Data[i], _Error, _SuppressHandler)) return false;

		return true;
	}
	CATCH_ERROR;
}

template<typename _Ty>
bool WriteVec3(
	_In_ Address _Address,
	_In_ Vec3<_Ty> _Data,
	_In_ const char* _Mask = "123",
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
) {
	try {
		if (_Data || !CheckVecMask(_Mask, 3)) throw Error(ERRORDEF, Error::ParameterInvalidError);

		Byte vec[3];

		for (size_t i = 0; i < 3; i++) vec[i] = _Mask[i] - 0x30;

		for (size_t i = 0; i < 3; i++) if (!Write(_Address + (sizeof(_Ty) * (vec[i] - 1)), _Data[i], _Error, _SuppressHandler)) return false;

		return true;
	}
	CATCH_ERROR;
}

template<typename _Ty>
bool WriteVec4(
	_In_ Address _Address,
	_In_ Vec4<_Ty> _Data,
	_In_ const char* _Mask = "1234",
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
) {
	try {
		if (!_Data || !CheckVecMask(_Mask, 4)) throw Error(ERRORDEF, Error::ParameterInvalidError);

		Byte vec[4];

		for (size_t i = 0; i < 4; i++) vec[i] = _Mask[i] - 0x30;

		for (size_t i = 0; i < 4; i++) if (!Write(_Address + (sizeof(_Ty) * (vec[i] - 1)), _Data[i], _Error, _SuppressHandler)) return false;

		return true;
	}
	CATCH_ERROR;
}

bool WriteStringA(
	_In_ Address _Address,
	_In_ const char* _String,
	_In_ bool _WriteNullTerm = true,
	_In_opt_ size_t _Cleanup = NULL,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

bool WriteStringW(
	_In_ Address _Address,
	_In_ const wchar_t* _String,
	_In_ bool _WriteNullTerm = true,
	_In_opt_ size_t _Cleanup = NULL,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

bool WriteBytes(
	_In_ Address _Address,
	_Out_writes_all_(_BufferSize) Byte* _Buffer,
	_In_ size_t _BufferSize,
	_In_opt_ size_t _Nop = NULL,
	_Out_writes_all_opt_(_ReturnBufferSize) Byte* _ReturnBuffer = nullptr,
	_In_ size_t _ReturnBufferSize = NULL,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

bool VQuery(
	_In_ Address _Address,
	_In_ MEMORY_BASIC_INFORMATION* _Mbi,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

bool VAlloc(
	_In_ Address _Address,
	_In_ size_t _Size,
	_In_ AllocationType _AllocationType,
	_In_ MemoryProtection _Protection,
	_Out_opt_ Address* _AllocationAddress = nullptr,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

bool VFree(
	_In_ Address _Address,
	_In_ size_t _Size = NULL,
	_In_ FreeType _FreeType = FreeType::Release,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

bool VProtect(
	_In_ Address _Address,
	_In_ size_t _Size,
	_In_ DWORD _Protection,
	_Out_opt_ DWORD* _OldProtection = nullptr,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

bool VProtect(
	_In_ Address _Address,
	_In_ size_t _Size,
	_In_ MemoryProtection _Protection,
	_Out_opt_ DWORD* _OldProtection = nullptr,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

bool ScanPattern(
	_In_ const char* _Pattern,
	_In_ const char* _Mask,
	_Out_ Address* _Return,
	_In_ Address _StartAddress = CE_MIN_ADDR,
	_In_ size_t _Size = CE_MAX_ADDR,
	_In_ size_t _Alignment = 1,
	_In_ ScanProtection _Protection = ScanProtection::Read,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

bool ScanPattern(
	_In_ const char* _ComboPattern,
	_Out_ Address* _Return,
	_In_ Address _StartAddress = CE_MIN_ADDR,
	_In_ size_t _Size = CE_MAX_ADDR,
	_In_ size_t _Alignment = 1,
	_In_ ScanProtection _Protection = ScanProtection::Read,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

bool ScanStringA(
	_In_ const char* _String,
	_Out_ Address* _Return,
	_In_ bool _CaseSensitive = true,
	_In_ Address _StartAddress = CE_MIN_ADDR,
	_In_ size_t _Size = CE_MAX_ADDR,
	_In_ size_t _Alignment = 1,
	_In_ ScanProtection _Protection = ScanProtection::Read,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

bool ScanStringW(
	_In_ const wchar_t* _String,
	_Out_ Address* _Return,
	_In_ bool _CaseSensitive = true,
	_In_ Address _StartAddress = CE_MIN_ADDR,
	_In_ size_t _Size = CE_MAX_ADDR,
	_In_ size_t _Alignment = 1,
	_In_ ScanProtection _Protection = ScanProtection::Read,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
);

template<typename _Ty>
bool ScanValue(
	_In_ _Ty _Value,
	_Out_ Address* _Return,
	_In_ Address _StartAddress = CE_MIN_ADDR,
	_In_ size_t _Size = CE_MAX_ADDR,
	_In_ size_t _Alignment = 1,
	_In_ ScanProtection _Protection = ScanProtection::Read,
	_Out_opt_ Error* _Error = nullptr,
	_In_ bool _SuppressHandler = false
) {
	try {
		if (!_Return) throw Error(ERRORDEF, Error::ReturnIsNullError);
		if (!IsProtectionValid(_Protection)) throw Error(ERRORDEF, Error::ParameterInvalidError);

		MEMORY_BASIC_INFORMATION mbi = { NULL };
		mbi.RegionSize = 4096;

		for (Address _CurrentAddress = _StartAddress; _CurrentAddress < _StartAddress + _Size; _CurrentAddress += mbi.RegionSize) {
			if (!VQuery(_CurrentAddress, &mbi, nullptr, true)) continue;
			if (mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS) continue;
			if (!ResolveProtection(_Protection, mbi.Protect)) continue;

			for (size_t i = 0; i < mbi.RegionSize; i += _Alignment) {
				if (i + sizeof(_Ty) >= mbi.RegionSize) break;

				if (_Value == *(_Ty*)(_CurrentAddress + i)) {
					*_Return = (_CurrentAddress + i);
					return true;
				}
			}
		}
		throw Error(ERRORDEF, Error::ScanValueNotFoundError);
	}
	CATCH_ERROR;
}

LIB_END

#endif