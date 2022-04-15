#include "../include/ExtMem.h"

LIB_BEGIN

External::External(
	_In_ const wchar_t* _ProcessName,
	_In_opt_ const wchar_t* _ModuleName,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if (hSnap == INVALID_HANDLE_VALUE) throw Error(ERRORDEF, Error::GenericWinApiFunctionError, true);

		PROCESSENTRY32W pe32 = { NULL };
		pe32.dwSize = sizeof(pe32);

		if (!Process32FirstW(hSnap, &pe32)) throw Error(ERRORDEF, Error::GenericWinApiFunctionError, { hSnap }, true);

		do {
			if (!_wcsicmp(pe32.szExeFile, _ProcessName)) {
				this->dwProcessId = pe32.th32ProcessID;
				break;
			}

		} while (Process32NextW(hSnap, &pe32));
		CloseHandle(hSnap);

		if (!this->dwProcessId) throw Error(ERRORDEF, Error::THEntryNotFoundError);

		this->hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, this->dwProcessId);
		if (!this->hProcess) throw Error(ERRORDEF, Error::GenericWinApiFunctionError, true);

		hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, this->dwProcessId);
		if (hSnap == INVALID_HANDLE_VALUE) throw Error(ERRORDEF, Error::GenericWinApiFunctionError, { this->hProcess }, true);

		MODULEENTRY32W me32 = { NULL };
		me32.dwSize = sizeof(me32);

		if (!Module32FirstW(hSnap, &me32)) throw Error(ERRORDEF, Error::GenericWinApiFunctionError, { hSnap, this->hProcess }, true);

		if (_ModuleName) {
			do {
				if (!_wcsicmp(me32.szModule, _ModuleName)) {
					this->hModule = me32.hModule;
					this->nModuleBase = (Address)me32.modBaseAddr;
					this->nModuleSize = (size_t)me32.modBaseSize;
					break;
				}

			} while (Module32NextW(hSnap, &me32));
		}

		else {
			do {
				if (!_wcsicmp(me32.szModule, _ProcessName)) {
					this->hModule = me32.hModule;
					this->nModuleBase = (Address)me32.modBaseAddr;
					this->nModuleSize = (size_t)me32.modBaseSize;
					break;
				}

			} while (Module32NextW(hSnap, &me32));
		}

		CloseHandle(hSnap);

		if (!this->nModuleBase) throw Error(ERRORDEF, Error::THEntryNotFoundError, { this->hProcess }, false);

		return;
	}
	catch (Error e) { if (_Error) *_Error = e; return; }
}

External::~External() { CloseHandle(this->hProcess); }

bool External::GetAddress(
	_In_ Address _StartAddress,
	_In_ Offsets _Offsets,
	_Out_ Address* _Return,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		if (!_Return) throw Error(ERRORDEF, Error::ReturnIsNullError);

		for (size_t i = 0; i < _Offsets.Size(); i++) {
			if (!this->Read(_StartAddress, &_StartAddress, _Error, _SuppressHandler)) return false;
			_StartAddress += _Offsets[i];
		}

		*_Return = _StartAddress;
		return true;
	}
	CATCH_ERROR;
}

bool External::ReadStringA(
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

		if (!ReadProcessMemory(this->hProcess, (LPCVOID)(_Address), _Buffer, _Length, nullptr)) throw Error(ERRORDEF, Error::WinReadError, true);

		_Buffer[_Length] = '\0';

		return true;
	}
	CATCH_ERROR;
}

bool External::ReadStringW(
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

		if (!ReadProcessMemory(this->hProcess, (LPCVOID)(_Address), _Buffer, (_Length * 2), nullptr)) throw Error(ERRORDEF, Error::WinReadError, true);

		_Buffer[_Length] = L'\0';

		return true;
	}
	CATCH_ERROR;
}

bool External::ReadBytes(
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

		if (!ReadProcessMemory(this->hProcess, (LPCVOID)(_Address), _Buffer, _Length, nullptr)) throw Error(ERRORDEF, Error::WinReadError, true);

		return true;
	}
	CATCH_ERROR;
}

bool External::WriteStringA(
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
		if (!WriteProcessMemory(this->hProcess, (LPVOID)(_Address), _String, _StringLen, nullptr)) throw Error(ERRORDEF, Error::WinWriteError, true);
		if (_WriteNullTerm) if (!this->Write(_Address + _StringLen, '\0', _Error, _SuppressHandler)) return false;
		for (size_t i = 0; i < _Cleanup; i++) if (!this->Write(_Address + _StringLen + i, '\0', _Error, _SuppressHandler)) return false;
		return true;
	}
	CATCH_ERROR;
}

bool External::WriteStringW(
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
		if (!WriteProcessMemory(this->hProcess, (LPVOID)(_Address), _String, (_StringLen * 2), nullptr)) throw Error(ERRORDEF, Error::WinWriteError, true);
		if (_WriteNullTerm) if (!this->Write(_Address + (_StringLen * 2), '\0', _Error, _SuppressHandler)) return false;
		for (size_t i = 0; i < _Cleanup; i++) if (!this->Write(_Address + ((_StringLen + i) * 2), '\0', _Error, _SuppressHandler)) return false;
		return true;
	}
	CATCH_ERROR;
}

bool External::WriteBytes(
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

		if (_ReturnBuffer) { if (!this->ReadBytes(_Address, _BufferSize + _Nop, _ReturnBuffer, _ReturnBufferSize, _Error, _SuppressHandler)) return false; }

#pragma warning(push)
#pragma warning(disable:6387)
		if (!WriteProcessMemory(this->hProcess, (LPVOID)(_Address), _Buffer, _BufferSize, nullptr)) throw Error(ERRORDEF, Error::WinWriteError, true);
#pragma warning(pop)
		for (size_t i = 0; i < _Nop; i++) if (!this->Write<Byte>(_Address + i, 0x90, _Error, _SuppressHandler)) return false;

		return true;
	}
	CATCH_ERROR;
}

bool External::VQuery(
	_In_ Address _Address,
	_In_ MEMORY_BASIC_INFORMATION* _Mbi,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		if (!VirtualQueryEx(this->hProcess, (LPCVOID)(_Address), _Mbi, sizeof(MEMORY_BASIC_INFORMATION))) throw Error(ERRORDEF, Error::VirtualQueryError, true);
		return true;
	}
	CATCH_ERROR;
}

bool External::VAlloc(
	_In_ Address _Address,
	_In_ size_t _Size,
	_In_ AllocationType _AllocationType,
	_In_ MemoryProtection _Protection,
	_Out_opt_ Address* _AllocationAddress,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		LPVOID _Result = VirtualAllocEx(this->hProcess, (LPVOID)(_Address), _Size, (DWORD)(_AllocationType), (DWORD)(_Protection));
		if (!_Result) throw Error(ERRORDEF, Error::VirtualAllocError, true);
		if (_AllocationAddress) *_AllocationAddress = (Address)(_Result);
		return true;
	}
	CATCH_ERROR;
}

bool External::VFree(
	_In_ Address _Address,
	_In_ size_t _Size,
	_In_ FreeType _FreeType,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
#pragma warning(push)
#pragma warning(disable:28160)
		if (!VirtualFreeEx(this->hProcess, (LPVOID)(_Address), _Size, (DWORD)(_FreeType))) throw Error(ERRORDEF, Error::VirtualFreeError, true);
#pragma warning(pop)
		return true;
	}
	CATCH_ERROR;
}

bool External::VProtect(
	_In_ Address _Address,
	_In_ size_t _Size,
	_In_ DWORD _Protection,
	_Out_opt_ DWORD* _OldProtection,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	try {
		DWORD _OldProtect;
		if (!VirtualProtectEx(this->hProcess, (LPVOID)(_Address), _Size, _Protection, &_OldProtect)) throw Error(ERRORDEF, Error::VirtualProtectError, true);
		if (_OldProtection) *_OldProtection = _OldProtect;
		return true;
	}
	CATCH_ERROR;
}
bool External::VProtect(
	_In_ Address _Address,
	_In_ size_t _Size,
	_In_ MemoryProtection _Protection,
	_Out_opt_ DWORD* _OldProtection,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	return this->VProtect(_Address, _Size, (DWORD)(_Protection), _OldProtection, _Error, _SuppressHandler);
}

bool External::ScanPattern(
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

		Byte* buf;

		for (Address _CurrentAddress = _StartAddress; _CurrentAddress < _StartAddress + _Size; _CurrentAddress += mbi.RegionSize) {
			if (!this->VQuery(_CurrentAddress, &mbi, nullptr, true)) continue;
			if (mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS) continue;
			if (!ResolveProtection(_Protection, mbi.Protect)) continue;

			buf = new Byte[mbi.RegionSize];
			if (!this->ReadBytes(_CurrentAddress, mbi.RegionSize, buf, mbi.RegionSize, nullptr, true)) {
				delete[] buf;
				continue;
			}

			for (size_t i = 0; i < mbi.RegionSize; i += _Alignment) {
				b = true;

				for (size_t j = 0; j < _PatternLen; j++) {
					if (i + j >= mbi.RegionSize) {
						b = false;
						break;
					}

					c = *(char*)(buf + i + j);

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

bool External::ScanPattern(
	_In_ const char* _ComboPattern,
	_Out_ Address* _Return,
	_In_ Address _StartAddress,
	_In_ size_t _Size,
	_In_ size_t _Alignment,
	_In_ ScanProtection _Protection,
	_Out_opt_ Error* _Error,
	_In_ bool _SuppressHandler
) {
	char _Pattern[MAX_PATTERN_LENGTH], _Mask[MAX_PATTERN_LENGTH];
	if (!ParsePattern(_ComboPattern, _Pattern, sizeof(_Pattern), _Mask, sizeof(_Mask), _Error, _SuppressHandler)) return false;
	return this->ScanPattern(_Pattern, _Mask, _Return, _StartAddress, _Size, _Alignment, _Protection, _Error, _SuppressHandler);
}

bool External::ScanStringA(
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
		char c;
		size_t _StrLen = strlen(_String);

		MEMORY_BASIC_INFORMATION mbi = { NULL };
		mbi.RegionSize = 4096;

		Byte* buf;

		for (Address _CurrentAddress = _StartAddress; _CurrentAddress < _StartAddress + _Size; _CurrentAddress += mbi.RegionSize) {
			if (!this->VQuery(_CurrentAddress, &mbi, nullptr, true)) continue;
			if (mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS) continue;
			if (!ResolveProtection(_Protection, mbi.Protect)) continue;

			buf = new Byte[mbi.RegionSize];
			if (!this->ReadBytes(_CurrentAddress, mbi.RegionSize, buf, mbi.RegionSize, nullptr, true)) {
				delete[] buf;
				continue;
			}

			for (size_t i = 0; i < mbi.RegionSize; i += _Alignment) {
				b = true;

				if (_CaseSensitive) {
					for (size_t j = 0; j < _StrLen; j++) {
						if (i + j >= mbi.RegionSize) {
							b = false;
							break;
						}

						if (_String[j] != *(char*)(buf + i + j)) {
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

						c = *(char*)(buf + i + j);

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

bool External::ScanStringW(
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

		Byte* buf;

		for (Address _CurrentAddress = _StartAddress; _CurrentAddress < _StartAddress + _Size; _CurrentAddress += mbi.RegionSize) {
			if (!this->VQuery(_CurrentAddress, &mbi, nullptr, true)) continue;
			if (mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS) continue;
			if (!ResolveProtection(_Protection, mbi.Protect)) continue;

			buf = new Byte[mbi.RegionSize];
			if (!this->ReadBytes(_CurrentAddress, mbi.RegionSize, buf, mbi.RegionSize, nullptr, true)) {
				delete[] buf;
				continue;
			}

			for (size_t i = 0; i < mbi.RegionSize; i += _Alignment) {
				b = true;

				if (_CaseSensitive) {
					for (size_t j = 0; j < _StrLen; j++) {
						if (i + j >= mbi.RegionSize) {
							b = false;
							break;
						}

						if (_String[j] != *(wchar_t*)(buf + i + (j * 2))) {
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

						c = *(wchar_t*)(buf + i + (j * 2));

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

LIB_END