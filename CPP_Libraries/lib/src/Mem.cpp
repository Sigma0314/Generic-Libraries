#include "../include/Mem.h"

LIB_BEGIN

bool GetProcessInfo(
	_Out_ ProcessInfo* _ProcessInfo,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		_ProcessInfo->hProcess = GetCurrentProcess();
		_ProcessInfo->dwProcessId = GetCurrentProcessId();
		_ProcessInfo->hModule = GetModuleHandleW(nullptr);
		_ProcessInfo->nModuleBase = (Address)(_ProcessInfo->hModule);

		MODULEINFO mnfo = { NULL };
		if (!K32GetModuleInformation(_ProcessInfo->hProcess, _ProcessInfo->hModule, &mnfo, sizeof(mnfo))) throw Error(ERRORDEF, Error::GenericWinApiFunctionError, true);
		_ProcessInfo->nModuleSize = mnfo.SizeOfImage;

		return true;
	}
	CATCH_ERROR;
}

bool GetModuleInfo(
	_In_ const wchar_t* _ModuleName,
	_Out_ ModuleInfo* _ModuleInfo,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		_ModuleInfo->hModule = GetModuleHandleW(_ModuleName);
		if (!_ModuleInfo->hModule) throw Error(ERRORDEF, Error::GenericWinApiFunctionError, true);

		_ModuleInfo->nModuleBase = (Address)(_ModuleInfo->hModule);

		MODULEINFO mnfo = { NULL };
		if (!K32GetModuleInformation(GetCurrentProcess(), _ModuleInfo->hModule, &mnfo, sizeof(mnfo))) throw Error(ERRORDEF, Error::GenericWinApiFunctionError, true);
		_ModuleInfo->nModuleSize = mnfo.SizeOfImage;

		return true;
	}
	CATCH_ERROR;
}

bool InitializeHook(
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		MH_STATUS st = MH_Initialize();
		if (st != MH_OK)
			if (st != MH_ERROR_MEMORY_ALLOC && st != MH_ERROR_MEMORY_PROTECT) throw Error(ERRORDEF, (Error::Code)(st));
			else throw Error(ERRORDEF, (Error::Code)(st), true);
		return true;
	}
	CATCH_ERROR;
}

bool UninitializeHook(
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	MH_STATUS st = MH_Uninitialize();
	if (st != MH_OK)
		if (st != MH_ERROR_MEMORY_ALLOC && st != MH_ERROR_MEMORY_PROTECT) throw Error(ERRORDEF, (Error::Code)(st));
		else throw Error(ERRORDEF, (Error::Code)(st), true);
	return true;
}

bool RemoveHook(
	_In_ LPVOID _Target,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		MH_STATUS st = MH_RemoveHook(_Target);
		if (st != MH_OK)
			if (st != MH_ERROR_MEMORY_ALLOC && st != MH_ERROR_MEMORY_PROTECT) throw Error(ERRORDEF, (Error::Code)(st));
			else throw Error(ERRORDEF, (Error::Code)(st), true);
		return true;
	}
	CATCH_ERROR;
}

bool EnableHook(
	_In_ LPVOID _Target,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		MH_STATUS st = MH_EnableHook(_Target);
		if (st != MH_OK)
			if (st != MH_ERROR_MEMORY_ALLOC && st != MH_ERROR_MEMORY_PROTECT) throw Error(ERRORDEF, (Error::Code)(st));
			else throw Error(ERRORDEF, (Error::Code)(st), true);
		return true;
	}
	CATCH_ERROR;
}

bool DisableHook(
	_In_ LPVOID _Target,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		MH_STATUS st = MH_DisableHook(_Target);
		if (st != MH_OK)
			if (st != MH_ERROR_MEMORY_ALLOC && st != MH_ERROR_MEMORY_PROTECT) throw Error(ERRORDEF, (Error::Code)(st));
			else throw Error(ERRORDEF, (Error::Code)(st), true);
		return true;
	}
	CATCH_ERROR;
}

bool GetAddress(
	_In_ Address _StartAddress,
	_In_ Offsets _Offsets,
	_Out_ Address* _Return,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		if (!_Return) throw Error(ERRORDEF, Error::ReturnIsNullError);

		for (size_t i = 0; i < _Offsets.Size(); i++) {
			if (!Read(_StartAddress, &_StartAddress, _Error, _SuppressHandler)) return false;
			_StartAddress += _Offsets[i];
		}

		*_Return = _StartAddress;
		return true;
	}
	CATCH_ERROR;
}

bool ReadStringA(
	_In_ Address _Address,
	_In_ size_t _Length,
	_Out_writes_all_(_ArrayLength) char* _Buffer,
	_In_ size_t _ArrayLength,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		if (!_Buffer) throw Error(ERRORDEF, Error::ReturnIsNullError);

		if (!(_ArrayLength > _Length)) throw Error(ERRORDEF, Error::BufferTooSmallError);

		char c;
		for (size_t i = 0; i < _Length; i++) {
			if (!Read(_Address + i, &c, _Error, _SuppressHandler)) return false;
			if (c == '\0') {
				_Buffer[i] = '\0';
				return true;
			}

			_Buffer[i] = c;
		}

		_Buffer[_Length] = '\0';
		return true;
	}
	CATCH_ERROR;
}

bool ReadStringW(
	_In_ Address _Address,
	_In_ size_t _Length,
	_Out_writes_all_(_ArrayLength) wchar_t* _Buffer,
	_In_ size_t _ArrayLength,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		if (!_Buffer) throw Error(ERRORDEF, Error::ReturnIsNullError);

		if (!(_ArrayLength > _Length)) throw Error(ERRORDEF, Error::BufferTooSmallError);

		wchar_t wc;
		for (size_t i = 0; i < _Length; i++) {
			if (!Read(_Address + (i * 2), &wc, _Error, _SuppressHandler)) return false;
			if (wc == L'\000') {
				_Buffer[i] = L'\000';
				return true;
			}

			_Buffer[i] = wc;
		}

		_Buffer[_Length] = L'\000';
		return true;
	}
	CATCH_ERROR;
}

bool ReadBytes(
	_In_ Address _Address,
	_In_ size_t _Length,
	_Out_writes_all_(_ArrayLength) Byte* _Buffer,
	_In_ size_t _ArrayLength,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		if (!_Buffer) throw Error(ERRORDEF, Error::ReturnIsNullError);

		if (_ArrayLength < _Length) throw Error(ERRORDEF, Error::BufferTooSmallError);

		Byte n;
		for (size_t i = 0; i < _Length; i++) {
			if (!Read(_Address + i, &n, _Error, _SuppressHandler)) return false;
			_Buffer[i] = n;
		}
		return true;
	}
	CATCH_ERROR;
}

bool WriteStringA(
	_In_ Address _Address,
	_In_ const char* _String,
	_In_ bool _WriteNullTerm,
	_In_opt_ size_t _Cleanup,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		if (!_String) throw Error(ERRORDEF, Error::ParameterInvalidError);

		size_t _StringLen = strlen(_String);
		for (size_t i = 0; i < _StringLen; i++) if (!Write(_Address + i, _String[i], _Error, _SuppressHandler)) return false;
		if (_WriteNullTerm) if (!Write(_Address + _StringLen, '\0', _Error, _SuppressHandler)) return false;
		for (size_t i = 0; i < _Cleanup; i++) if (!Write(_Address + _StringLen + i, '\0', _Error, _SuppressHandler)) return false;
		return true;

	}
	CATCH_ERROR;
}

bool WriteStringW(
	_In_ Address _Address,
	_In_ const wchar_t* _String,
	_In_ bool _WriteNullTerm,
	_In_opt_ size_t _Cleanup,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		if (!_String) throw Error(ERRORDEF, Error::ParameterInvalidError);

		size_t _StringLen = lstrlenW(_String);
		for (size_t i = 0; i < _StringLen; i++) if (!Write(_Address + (i * 2), _String[i], _Error, _SuppressHandler)) return false;
		if (_WriteNullTerm) if (!Write(_Address + (_StringLen * 2), '\0', _Error, _SuppressHandler)) return false;
		for (size_t i = 0; i < _Cleanup; i++) if (!Write(_Address + ((_StringLen + i) * 2), '\0', _Error, _SuppressHandler)) return false;
		return true;
	}
	CATCH_ERROR;
}

bool WriteBytes(
	_In_ Address _Address,
	_Out_writes_all_(_BufferSize) Byte* _Buffer,
	_In_ size_t _BufferSize,
	_In_opt_ size_t _Nop,
	_Out_writes_all_opt_(_ReturnBufferSize) Byte* _ReturnBuffer,
	_In_ size_t _ReturnBufferSize,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		if (!_Buffer && !_Nop) throw Error(ERRORDEF, Error::ParameterInvalidError);

		if (_ReturnBuffer) { if (!ReadBytes(_Address, _BufferSize + _Nop, _ReturnBuffer, _ReturnBufferSize, _Error, _SuppressHandler)) return false; }

#pragma warning(push)
#pragma warning(disable:6011)
		for (size_t i = 0; i < _BufferSize; i++) if (!Write(_Address + i, _Buffer[i], _Error, _SuppressHandler)) return false;
#pragma warning(pop)
		for (size_t i = 0; i < _Nop; i++) if (!Write<Byte>(_Address + i, 0x90, _Error, _SuppressHandler)) return false;

		return true;
	}
	CATCH_ERROR;
}

bool VQuery(
	_In_ Address _Address,
	_In_ MEMORY_BASIC_INFORMATION* _Mbi,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		if (!VirtualQuery((LPCVOID)(_Address), _Mbi, sizeof(MEMORY_BASIC_INFORMATION))) throw Error(ERRORDEF, Error::VirtualQueryError, true);
		return true;
	}
	CATCH_ERROR;
}

bool VAlloc(
	_In_ Address _Address,
	_In_ size_t _Size,
	_In_ AllocationType _AllocationType,
	_In_ MemoryProtection _Protection,
	_Out_opt_ Address* _AllocationAddress,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		LPVOID _Result = VirtualAlloc((LPVOID)(_Address), _Size, (DWORD)(_AllocationType), (DWORD)(_Protection));
		if (!_Result) throw Error(ERRORDEF, Error::VirtualAllocError, true);
		if (_AllocationAddress) *_AllocationAddress = (Address)(_Result);
		return true;
	}
	CATCH_ERROR;
}

bool VFree(
	_In_ Address _Address,
	_In_ size_t _Size,
	_In_ FreeType _FreeType,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
#pragma warning(push)
#pragma warning(disable:28160)
		if (!VirtualFree((LPVOID)(_Address), _Size, (DWORD)(_FreeType))) throw Error(ERRORDEF, Error::VirtualFreeError, true);
#pragma warning(pop)
		return true;
	}
	CATCH_ERROR;
}

bool VProtect(
	_In_ Address _Address,
	_In_ size_t _Size,
	_In_ DWORD _Protection,
	_Out_opt_ DWORD* _OldProtection,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		DWORD _OldProtect;
		if (!VirtualProtect((LPVOID)(_Address), _Size, _Protection, &_OldProtect)) throw Error(ERRORDEF, Error::VirtualProtectError, true);
		if (_OldProtection) *_OldProtection = _OldProtect;
		return true;
	}
	CATCH_ERROR;
}
bool VProtect(
	_In_ Address _Address,
	_In_ size_t _Size,
	_In_ MemoryProtection _Protection,
	_Out_opt_ DWORD* _OldProtection,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	return VProtect(_Address, _Size, (DWORD)(_Protection), _OldProtection, _Error, _SuppressHandler);
}

bool ScanPattern(
	_In_ const char* _Pattern,
	_In_ const char* _Mask,
	_Out_ Address* _Return,
	_In_ Address _StartAddress,
	_In_ size_t _Size,
	_In_ size_t _Alignment,
	_In_ ScanProtection _Protection,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		if (!_Return) throw Error(ERRORDEF, Error::ReturnIsNullError);
		if (!IsProtectionValid(_Protection)) throw Error(ERRORDEF, Error::ParameterInvalidError);

		bool b;
		char c;
		size_t _PatternLen = strlen(_Mask);

		MEMORY_BASIC_INFORMATION mbi = { NULL };
		mbi.RegionSize = 4096;

		for (Address _CurrentAddress = _StartAddress; _CurrentAddress < _StartAddress + _Size; _CurrentAddress += mbi.RegionSize) {
			if (!VQuery(_CurrentAddress, &mbi, nullptr, true)) continue;
			if (mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS) continue;
			if (!ResolveProtection(_Protection, mbi.Protect)) continue;

			for (size_t i = 0; i < mbi.RegionSize; i += _Alignment) {
				b = true;

				for (size_t j = 0; j < _PatternLen; j++) {
					if (i + j >= mbi.RegionSize) {
						b = false;
						break;
					}

					c = *(char*)(_CurrentAddress + i + j);

					switch (_Mask[j]) {
					case '?': continue;
					case 'x':
						if (_Pattern[j] != c) break;
						else continue;
					case 'l':
						if (LO_NIBBLE(_Pattern[j]) != LO_NIBBLE(c)) break;
						else continue;
					case 'h':
						if (HI_NIBBLE(_Pattern[j]) != HI_NIBBLE(c)) break;
						else continue;
					}

					b = false;
					break;
				}

				if (b) {
					*_Return = (_CurrentAddress + i);
					return true;
				}
			}
		}
		throw Error(ERRORDEF, Error::ScanValueNotFoundError);
	}
	CATCH_ERROR;
}

bool ScanPattern(
	_In_ const char* _ComboPattern,
	_Out_ Address* _Return,
	_In_ Address _StartAddress,
	_In_ size_t _Size,
	_In_ size_t _Alignment,
	_In_ ScanProtection _Protection,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	char* _Pattern, * _Mask;
	if (!VAlloc(NULL, MAX_PATTERN_LENGTH, AllocationType::Commit_Reserve_TopDown, MemoryProtection::ReadWrite, (Address*)&_Pattern, _Error, _SuppressHandler)) return false;
	if (!VAlloc(NULL, MAX_PATTERN_LENGTH, AllocationType::Commit_Reserve_TopDown, MemoryProtection::ReadWrite, (Address*)&_Mask, _Error, _SuppressHandler)) return false;

	if (!ParsePattern(_ComboPattern, _Pattern, MAX_PATTERN_LENGTH, _Mask, MAX_PATTERN_LENGTH, _Error, _SuppressHandler)) return false;
	bool _Result = ScanPattern(_Pattern, _Mask, _Return, _StartAddress, _Size, _Alignment, _Protection, _Error, _SuppressHandler);

	if (!VFree((Address)_Pattern, NULL, FreeType::Release, _Error, _SuppressHandler)) return false;
	if (!VFree((Address)_Mask, NULL, FreeType::Release, _Error, _SuppressHandler)) return false;

	return _Result;
}

bool ScanStringA(
	_In_ const char* _String,
	_Out_ Address* _Return,
	_In_ bool _CaseSensitive,
	_In_ Address _StartAddress,
	_In_ size_t _Size,
	_In_ size_t _Alignment,
	_In_ ScanProtection _Protection,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		if (!_Return) throw Error(ERRORDEF, Error::ReturnIsNullError);
		if (!IsProtectionValid(_Protection)) throw Error(ERRORDEF, Error::ParameterInvalidError);

		bool b;
		size_t _StrLen = strlen(_String);

		MEMORY_BASIC_INFORMATION mbi = { NULL };
		mbi.RegionSize = 4096;

		for (Address _CurrentAddress = _StartAddress; _CurrentAddress < _StartAddress + _Size; _CurrentAddress += mbi.RegionSize) {
			if (!VQuery(_CurrentAddress, &mbi, nullptr, true)) continue;
			if (mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS) continue;
			if (!ResolveProtection(_Protection, mbi.Protect)) continue;

			char c;

			for (size_t i = 0; i < mbi.RegionSize; i += _Alignment) {
				b = true;

				if (_CaseSensitive) {
					for (size_t j = 0; j < _StrLen; j++) {
						if (i + j >= mbi.RegionSize) {
							b = false;
							break;
						}

						if (_String[j] != *(char*)(_CurrentAddress + i + j)) {
							b = false;
							break;
						}
					}
				}
				else {
					for (size_t j = 0; j < _StrLen; j++) {
						if (i + j >= mbi.RegionSize) {
							b = false;
							break;
						}

						c = *(char*)(_CurrentAddress + i + j);

						if (IS_LOWERCASE(_String[j])) {
							if (_String[j] != c && _String[j] != TO_UPPERCASE(c)) {
								b = false;
								break;
							}
						}
						else if (IS_UPPERCASE(_String[j])) {
							if (_String[j] != c && _String[j] != TO_LOWERCASE(c)) {
								b = false;
								break;
							}
						}
						else {
							if (_String[j] != c) {
								b = false;
								break;
							}
						}
					}
				}

				if (b) {
					*_Return = (_CurrentAddress + i);
					return true;
				}
			}
		}
		throw Error(ERRORDEF, Error::ScanValueNotFoundError);
	}
	CATCH_ERROR;
}

bool ScanStringW(
	_In_ const wchar_t* _String,
	_Out_ Address* _Return,
	_In_ bool _CaseSensitive,
	_In_ Address _StartAddress,
	_In_ size_t _Size,
	_In_ size_t _Alignment,
	_In_ ScanProtection _Protection,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		if (!_Return) throw Error(ERRORDEF, Error::ReturnIsNullError);
		if (!IsProtectionValid(_Protection)) throw Error(ERRORDEF, Error::ParameterInvalidError);

		bool b;
		wchar_t c;
		size_t _StrLen = lstrlenW(_String);

		MEMORY_BASIC_INFORMATION mbi = { NULL };
		mbi.RegionSize = 4096;

		for (Address _CurrentAddress = _StartAddress; _CurrentAddress < _StartAddress + _Size; _CurrentAddress += mbi.RegionSize) {
			if (!VQuery(_CurrentAddress, &mbi, nullptr, true)) continue;
			if (mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS) continue;
			if (!ResolveProtection(_Protection, mbi.Protect)) continue;

			for (size_t i = 0; i < mbi.RegionSize; i += _Alignment) {
				b = true;

				if (_CaseSensitive) {
					for (size_t j = 0; j < _StrLen; j++) {
						if (i + j >= mbi.RegionSize) {
							b = false;
							break;
						}

						if (_String[j] != *(wchar_t*)(_CurrentAddress + i + (j * 2))) {
							b = false;
							break;
						}
					}
				}
				else {
					for (size_t j = 0; j < _StrLen; j++) {
						if (i + (j * 2) >= mbi.RegionSize) {
							b = false;
							break;
						}

						c = *(wchar_t*)(_CurrentAddress + i + (j * 2));

						if (IS_LOWERCASE(_String[j])) {
							if (_String[j] != c && _String[j] != TO_UPPERCASE(c)) {
								b = false;
								break;
							}
						}
						else if (IS_UPPERCASE(_String[j])) {
							if (_String[j] != c && _String[j] != TO_LOWERCASE(c)) {
								b = false;
								break;
							}
						}
						else {
							if (_String[j] != c) {
								b = false;
								break;
							}
						}
					}
				}

				if (b) {
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