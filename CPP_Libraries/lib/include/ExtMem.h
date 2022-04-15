#pragma once
#include <Windows.h>
#include <TlHelp32.h>

#include "EnvVar.h"
#include "Error.h"
#include "MemShared.h"
#include "Collections.h"

#undef GetModuleHandle

LIB_BEGIN

class External {
	HANDLE hProcess;
	DWORD dwProcessId;
	HMODULE hModule;
	Address nModuleBase;
	size_t nModuleSize;

public:

	External() : hProcess(nullptr), dwProcessId(NULL), hModule(nullptr), nModuleBase(NULL), nModuleSize(NULL) {}
	External(
		_In_ const wchar_t* _ProcessName,
		_In_opt_ const wchar_t* _ModuleName = nullptr,
		_Out_opt_ Error* _Error = nullptr,
		_In_ bool _SuppressHandler = false
	);
	~External();

	HANDLE GetProcessHandle() const { return this->hProcess; }
	DWORD GetProcessId() const { return this->dwProcessId; }
	HMODULE GetModuleHandle() const { return this->hModule; }
	Address GetModuleBase() const { return this->nModuleBase; }
	size_t GetModuleSize() const { return this->nModuleSize; }

	template<typename _Ty>
	bool Read(
		_In_ Address _Address,
		_Out_ _Ty* _Return,
		_Out_opt_ Error* _Error = nullptr,
		_In_ bool _SuppressHandler = false
	) {
		try {
			if (!_Return) throw Error(ERRORDEF, Error::ReturnIsNullError);

			_Ty _Buffer;
			if (!ReadProcessMemory(this->hProcess, (LPCVOID)(_Address), &_Buffer, sizeof(_Buffer), nullptr)) throw Error(ERRORDEF, Error::WinReadError, true);

			*_Return = _Buffer;
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

			if (!this->GetAddress(_StartAddress, _Offsets, &_StartAddress, _Error, _SuppressHandler)) return false;

			return this->Read(_StartAddress, _Return, _Error, _SuppressHandler);
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
	bool Write(
		_In_ Address _Address,
		_In_ _Ty _Data,
		_Out_opt_ Error* _Error = nullptr,
		_In_ bool _SuppressHandler = false
	) {
		try {
			if (!WriteProcessMemory(this->hProcess, (LPVOID)(_Address), &_Data, sizeof(_Data), nullptr)) throw Error(ERRORDEF, Error::WinWriteError, true);
			return true;
		}
		CATCH_ERROR;
	}

	template<typename _Ty>
	bool WritePtr(
		_In_ Address _StartAddress,
		_In_ Offsets _Offsets,
		_In_ _Ty _Data,
		_Out_opt_ Error* _Error = nullptr,
		_In_ bool _SuppressHandler = false
	) {
		if (!this->GetAddress(_StartAddress, _Offsets, &_StartAddress, _Error, _SuppressHandler)) return false;
		return this->Write(_StartAddress, _Data, _Error, _SuppressHandler);
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
			if (!CheckVecMask(_Mask, 2)) throw Error(ERRORDEF, Error::ParameterInvalidError);

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
			if (!CheckVecMask(_Mask, 3)) throw Error(ERRORDEF, Error::ParameterInvalidError);

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
			if (!CheckVecMask(_Mask, 4)) throw Error(ERRORDEF, Error::ParameterInvalidError);

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

			Byte* buf;

			for (Address _CurrentAddress = _StartAddress; _CurrentAddress < _StartAddress + _Size; _CurrentAddress += mbi.RegionSize) {
				if (!VQuery(_CurrentAddress, &mbi, nullptr, true)) continue;
				if (mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS) continue;
				if (!ResolveProtection(_Protection, mbi.Protect)) continue;

				buf = new Byte[mbi.RegionSize];
				if (!this->ReadBytes(_CurrentAddress, mbi.RegionSize, buf, mbi.RegionSize, nullptr, true)) {
					delete[] buf;
					continue;
				}

				for (size_t i = 0; i < mbi.RegionSize; i += _Alignment) {
					if (i + sizeof(_Ty) >= mbi.RegionSize) break;

					if (_Value == *(_Ty*)(buf + i)) {
						*_Return = (_CurrentAddress + i);
						delete[] buf;
						return true;
					}
				}

				delete[] buf;
			}
			throw Error(ERRORDEF, Error::ScanValueNotFoundError);
		}
		CATCH_ERROR;
	}
};

LIB_END