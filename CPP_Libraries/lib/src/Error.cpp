#include "../include/Error.h"

LIB_BEGIN

// void(__fastcall* _ErrorHandler)(Error* e) = nullptr;
// void SetErrorHandler(void(__fastcall* _Handler)(Error*)) { _ErrorHandler = _Handler; }

ErrorHandlerTy _ErrorHandler;
ErrorHandlerTy& ErrorHandler() { return _ErrorHandler; }

#pragma warning(push)
#pragma warning(disable:26812)
bool Error::GetErrorMessageFromCode(lib::Error::Code _ErrorCode, char* _Buffer, size_t _Size) {
	try {
		switch (_ErrorCode) {
		case Error::MH_UNKNOWN:
			throw "Unknown MinHook error.";
		case Error::MH_OK:
			throw "MinHook successful.";
		case Error::MH_ERROR_ALREADY_INITIALIZED:
			throw "MinHook is already initialized.";
		case Error::MH_ERROR_NOT_INITIALIZED:
			throw "MinHook is not initialized yet, or already uninitialized.";
		case Error::MH_ERROR_ALREADY_CREATED:
			throw "The hook for the specified target function is already created.";
		case Error::MH_ERROR_NOT_CREATED:
			throw "The hook for the specified target function is not created yet.";
		case Error::MH_ERROR_ENABLED:
			throw "The hook for the specified target function is already enabled.";
		case Error::MH_ERROR_DISABLED:
			throw "The hook for the specified target function is not enabled yet, or already disabled.";
		case Error::MH_ERROR_NOT_EXECUTABLE:
			throw "The specified pointer is invalid. It points the address of non-allocated and/or non-executable region.";
		case Error::MH_ERROR_UNSUPPORTED_FUNCTION:
			throw "The specified target function cannot be hooked.";
		case Error::MH_ERROR_FUNCTION_NOT_FOUND:
			throw "The specified function could not be found.";

		case Error::BufferTooSmallError:
			throw "The referenced buffer is too small for the operation executed by this function.";
		case Error::GenericWinApiFunctionError:
			throw "A Windows API function has failed unexpectedly. Call the GetWindowsError() member method for more error information.";
		case Error::ParameterInvalidError:
			throw "A parameter has a value that is deemed as invalid by this function.";
		case Error::ReadError:
			throw "A read operation has raised an exception and failed.";
		case Error::ReturnIsNullError:
			throw "The return parameter is null, but is required to contain a reference to a buffer where data shall be returned.";
		case Error::ScanValueNotFoundError:
			throw "The value to scan for has not been located anywhere in the specified scan range. Double check the parameters, or mute this error.";
		case Error::MH_ERROR_MODULE_NOT_FOUND:
		case Error::THEntryNotFoundError:
			throw "An entry of the process/module looked for has not been found. Double check the names and try again.";
		case Error::MH_ERROR_MEMORY_ALLOC:
		case Error::VirtualAllocError:
			throw "VirtualAlloc has failed unexpectedly. Call the GetWindowsError() member method for more error information.";
		case Error::VirtualFreeError:
			throw "VirtualFree has failed unexpectedly. Call the GetWindowsError() member method for more error information.";
		case Error::MH_ERROR_MEMORY_PROTECT:
		case Error::VirtualProtectError:
			throw "VirtualProtect has failed unexpectedly. Call the GetWindowsError() member method for more error information.";
		case Error::VirtualQueryError:
			throw "VirtualQuery has failed unexpectedly. Call the GetWindowsError() member method for more error information.";
		case Error::WinReadError:
			throw "A read operation has failed. Call the GetWindowsError() member method for more error information.";
		case Error::WinWriteError:
			throw "A write operation has failed. Call the GetWindowsError() member method for more error information.";
		case Error::WriteError:
			throw "A write operation has raised an exception and failed.";
		case Error::ArchitectureMismatchError:
			throw "The architecture of the library and the architecture of the executable do not match.";
		case Error::FunctionFailedError:
			throw "A templated function call failed unexpectedly.";
		case Error::InvalidFilePathError:
			throw "The file path specified is invalid.";
		case Error::UnknownError:
			throw "An unknown error has occured.";

		default:
			return false;
		}
	}
	catch (const char* _Str) {
		if (strcpy_s(_Buffer, _Size, _Str)) return false;
		else return true;
	}
}
#pragma warning(pop)

Error::Error(_In_ bool _SuppressHandler, _In_ const char* _FunctionName, _In_ const char* _File, _In_ size_t _Line, _In_ Error::Code _ErrorCode) : _Failed(true), _LineNumber(_Line), _ErrorCode(_ErrorCode), _WinErrorCode(NULL) {
	strcpy_s(this->_FunctionName, _FunctionName);
	strcpy_s(this->_FilePath, _File);

	Error::GetErrorMessageFromCode(_ErrorCode, this->_ErrorMessage, sizeof(this->_ErrorMessage));

	this->_WinErrorMessage[0] = '\0';

	if (!_SuppressHandler) _ErrorHandler.Invoke(this);
}

Error::Error(_In_ bool _SuppressHandler, _In_ const char* _FunctionName, _In_ const char* _File, _In_ size_t _Line, _In_ Error::Code _ErrorCode, _In_ bool _IsWinApi) : _Failed(true), _LineNumber(_Line), _ErrorCode(_ErrorCode) {
	strcpy_s(this->_FunctionName, _FunctionName);
	strcpy_s(this->_FilePath, _File);

	Error::GetErrorMessageFromCode(_ErrorCode, this->_ErrorMessage, sizeof(this->_ErrorMessage));

	if (_IsWinApi) {
		this->_WinErrorCode = GetLastError();
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, this->_WinErrorCode, LANG_SYSTEM_DEFAULT, this->_WinErrorMessage, sizeof(this->_WinErrorMessage), nullptr);
	}
	else {
		this->_WinErrorCode = NULL;
		this->_WinErrorMessage[0] = '\0';
	}

	if (!_SuppressHandler) _ErrorHandler.Invoke(this);
}

Error::Error(_In_ bool _SuppressHandler, _In_ const char* _FunctionName, _In_ const char* _File, _In_ size_t _Line, _In_ Error::Code _ErrorCode, _In_opt_ const Handles _Handles, _In_ bool _IsWinApi) : _Failed(true), _LineNumber(_Line), _ErrorCode(_ErrorCode) {
	strcpy_s(this->_FunctionName, _FunctionName);
	strcpy_s(this->_FilePath, _File);

	Error::GetErrorMessageFromCode(_ErrorCode, this->_ErrorMessage, sizeof(this->_ErrorMessage));

	for (size_t i = 0; i < _Handles.Size(); i++) CloseHandle(_Handles[i]);

	if (_IsWinApi) {
		this->_WinErrorCode = GetLastError();
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, this->_WinErrorCode, LANG_SYSTEM_DEFAULT, this->_WinErrorMessage, sizeof(this->_WinErrorMessage), nullptr);
	}
	else {
		this->_WinErrorCode = NULL;
		this->_WinErrorMessage[0] = '\0';
	}

	if (!_SuppressHandler) _ErrorHandler.Invoke(this);
}

LIB_END