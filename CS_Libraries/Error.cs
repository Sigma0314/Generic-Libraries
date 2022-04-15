using System;
using System.Runtime.CompilerServices;

namespace Lib
{
    namespace Error
    {
        class ErrorMessage
        {
            public const string str = "gay";
        }

        class LibraryExceptionHandlers
        {
            public delegate void tLibraryExceptionHandler(LibraryException e);

            public static event tLibraryExceptionHandler? LibraryExceptionHandler;

            public static void InvokeExceptionHandler(LibraryException e) => LibraryExceptionHandler?.Invoke(e);
        }

        [Serializable]
        class LibraryException : Exception
        {
            public LibraryException() { }

            public LibraryException(string message) : base(message) { }

            public LibraryException(string message, Exception inner) : base(message, inner) { }

            public LibraryException(
                string message,
                bool suppressHandler,
                bool isWinApi = false,
                IntPtr[]? handles = null,
                [CallerMemberName] string? functionName = null,
                [CallerFilePath] string? filePath = null,
                [CallerLineNumber] ulong? lineNumber = null
                ) : base(message)
            {
                _functionName = functionName;
                _filePath = filePath;
                _lineNumber = lineNumber;

                if (!suppressHandler) LibraryExceptionHandlers.InvokeExceptionHandler(this);
            }

            private string? _winMessage;
            public string? WinMessage { get { return _winMessage; } }

            private string? _functionName;
            public string? FunctionName { get { return _functionName; } }

            private string? _filePath;
            public string? FilePath { get { return _filePath; } }

            private ulong? _lineNumber;
            public ulong? LineNumber { get { return _lineNumber;} }
        }
    }
}
