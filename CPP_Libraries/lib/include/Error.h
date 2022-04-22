#pragma once
#include <Windows.h> // DWORD, MAX_PATH
#include <assert.h> // assert()

#include "EnvVar.h"
#include "Const.h"

#define ERROR_HANDLER(_FnName) void __fastcall _FnName(NS_NAME::Error* e)
#define CATCH_ERROR catch (Error &e) { if (_Error) *_Error = e; return false; }
#define ERRORDEF _SuppressHandler, __FUNCTION__, __FILE__, __LINE__

LIB_BEGIN

class Error {
public:
	enum Code {
		MH_UNKNOWN = -1,
		MH_OK = 0,
		MH_ERROR_ALREADY_INITIALIZED,
		MH_ERROR_NOT_INITIALIZED,
		MH_ERROR_ALREADY_CREATED,
		MH_ERROR_NOT_CREATED,
		MH_ERROR_ENABLED,
		MH_ERROR_DISABLED,
		MH_ERROR_NOT_EXECUTABLE,
		MH_ERROR_UNSUPPORTED_FUNCTION,
		MH_ERROR_MEMORY_ALLOC,
		MH_ERROR_MEMORY_PROTECT,
		MH_ERROR_MODULE_NOT_FOUND,
		MH_ERROR_FUNCTION_NOT_FOUND,

		ReturnIsNullError,
		ParameterInvalidError,
		ReadError,
		WriteError,
		WinReadError,
		WinWriteError,
		BufferTooSmallError,
		VirtualQueryError,
		VirtualAllocError,
		VirtualFreeError,
		VirtualProtectError,
		GenericWinApiFunctionError,
		ScanValueNotFoundError,
		THEntryNotFoundError,
		ArchitectureMismatchError,
		FunctionFailedError,
		InvalidFilePathError
	};

	Error() : _ErrorCode((Error::Code)NULL), _WinErrorCode(NULL), _Failed(false), _LineNumber(NULL) { this->_ErrorMessage[0] = '\0'; this->_WinErrorMessage[0] = '\0'; this->_FunctionName[0] = '\0'; this->_FilePath[0] = '\0'; }
	Error(_In_ bool _SuppressHandler, _In_ const char* _FunctionName, _In_ const char* _File, _In_ size_t _Line, _In_ Error::Code _ErrorCode);
	Error(_In_ bool _SuppressHandler, _In_ const char* _FunctionName, _In_ const char* _File, _In_ size_t _Line, _In_ Error::Code _ErrorCode, _In_ bool _IsWinApi);
	Error(_In_ bool _SuppressHandler, _In_ const char* _FunctionName, _In_ const char* _File, _In_ size_t _Line, _In_ Error::Code _ErrorCode, _In_opt_ const Handles _Handles, _In_ bool _IsWinApi);

	bool IsError() const { return this->_Failed; }

	const char* GetFunctionName() const { return this->_FunctionName; }
	const char* GetFileName() const { return this->_FilePath; }
	size_t GetLine() const { return this->_LineNumber; }

	Error::Code GetErrorCode() const { return this->_ErrorCode; }
	const char* GetErrorMessage() const { return this->_ErrorMessage; }

	DWORD GetWinErrorCode() const { return this->_WinErrorCode; }
	const char* GetWinErrorMessage() const { return this->_WinErrorMessage; }

private:
	bool _Failed;
	char _FunctionName[256];
	char _FilePath[MAX_PATH];
	size_t _LineNumber;
	char _ErrorMessage[256];
	Code _ErrorCode;
	char _WinErrorMessage[256];
	DWORD _WinErrorCode;

	static bool GetErrorMessageFromCode(Code _ErrorCode, char* _Buffer, size_t _Size);
};

inline int operator~(Error::Code _Code) { return (int)_Code; }

class ErrorHandlerTy {
	using _HandlerTy = void(__fastcall*)(Error*);

	_HandlerTy _HandlerArray[128];
	size_t _Count;

public:

	ErrorHandlerTy() : _Count(0) { memset(_HandlerArray, NULL, sizeof(_HandlerArray)); }

	void Invoke(Error* _This) {
		for (int i = 0; i < 128; i++)
			if (this->_HandlerArray[i])
				this->_HandlerArray[i](_This);
	}

	void operator+=(_HandlerTy _Handler) {
		assert(_Count < 128);
		for (int i = 0; i < 128; i++)
			if (!_HandlerArray[i]) {
				_HandlerArray[i] = _Handler;
				break;
			}
		this->_Count++;
	}

	void operator-=(_HandlerTy _Handler) {
		assert(_Count > 0);
		for (int i = 0; i < 128; i++)
			if (_HandlerArray[i] == _Handler) {
				_HandlerArray[i] = nullptr;
				break;
			}
		this->_Count--;
	};
};

ErrorHandlerTy& ErrorHandler();

LIB_END