using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Lib
{
    namespace Error
    {
        class ErrorMessage
        {
            public const string ProcessEntryNotFoundError = "An entry of the process looked for has not been found. Double check the names and try again.";
            public const string ModuleEntryNotFoundError = "An entry of the module looked for has not been found. Double check the names and try again.";
            public const string OpenProcessError = "OpenProcess failed. See WinApi error.";
            public const string WinReadError = "A read operation has failed. See WinApi error.";
        }

        class LibraryExceptionHandlers
        {
            public delegate void tLibraryExceptionHandler(LibraryException e);

            public static event tLibraryExceptionHandler? LibraryExceptionHandler;

            public static void InvokeExceptionHandler(LibraryException e) => LibraryExceptionHandler?.Invoke(e);
        }

        class LibraryExceptionFlags
        {
            public LibraryExceptionFlags(bool suppressHandler, bool isWinApi = false, [CallerMemberName] string? functionName = null, [CallerFilePath] string? filePath = null, [CallerLineNumber] ulong? lineNumber = null)
            {
                SuppressHandler = suppressHandler;
                IsWinApi = isWinApi;
                FunctionName = functionName;
                FilePath = filePath;
                LineNumber = lineNumber;
            }

            public bool SuppressHandler { get; private set; }
            public bool IsWinApi { get; private set; }

            public string? FunctionName { get; private set; }
            public string? FilePath { get; private set; }
            public ulong? LineNumber { get; private set; }
        }

        [Serializable]
        class LibraryException : Exception
        {
            [DllImport("kernel32.dll", CallingConvention = CallingConvention.StdCall, SetLastError = true)]
            private static extern uint FormatMessageA(uint dwFlags, IntPtr lpSource, uint dwMessageId, uint dwLanguageId, byte[] lpBuffer, uint nSize, IntPtr Arguments);

            [DllImport("kernel32.dll", CallingConvention = CallingConvention.StdCall, SetLastError = true)]
            private static extern bool CloseHandle(IntPtr hObject);

            public LibraryException() { IsError = false; }

            public LibraryException(LibraryException? _Prev, bool suppressHandler)
            {
                if (_Prev != null)
                {
                    IsError = true;
                    FunctionName = _Prev.FunctionName;
                    FilePath = _Prev.FilePath;
                    LineNumber = _Prev.LineNumber;
                    IsWinError = _Prev.IsWinError;
                    WinMessage = _Prev.WinMessage;
                }

                if (!suppressHandler) LibraryExceptionHandlers.InvokeExceptionHandler(this);
            }

            public LibraryException(
                string message,
                bool suppressHandler,
                bool isWinApi = false,
                [CallerMemberName] string? functionName = null,
                [CallerFilePath] string? filePath = null,
                [CallerLineNumber] ulong? lineNumber = null
                ) : base(message)
            {
                IsError = true;
                FunctionName = functionName;
                FilePath = filePath;
                LineNumber = lineNumber;
                IsWinError = isWinApi;

                if (isWinApi)
                {
                    byte[] buffer = new byte[128];

                    FormatMessageA(0x00001000, IntPtr.Zero, (uint)Marshal.GetLastWin32Error(), (0x02 << 10) | 0x00, buffer, 128, IntPtr.Zero);
                    WinMessage = System.Text.Encoding.ASCII.GetString(buffer);
                }

                else WinMessage = null;

                if (!suppressHandler) LibraryExceptionHandlers.InvokeExceptionHandler(this);
            }

            public LibraryException(
                string message,
                LibraryExceptionFlags libExceptionFlags,
                params IntPtr[] handles
                ) : base(message)
            {
                IsError = true;
                FunctionName = libExceptionFlags.FunctionName;
                FilePath = libExceptionFlags.FilePath;
                LineNumber = libExceptionFlags.LineNumber;
                IsWinError = libExceptionFlags.IsWinApi;

                if (libExceptionFlags.IsWinApi)
                {
                    byte[] buffer = new byte[128];

                    FormatMessageA(0x00001000, IntPtr.Zero, (uint)Marshal.GetLastWin32Error(), (0x02 << 10) | 0x00, buffer, 128, IntPtr.Zero);
                    WinMessage = System.Text.Encoding.ASCII.GetString(buffer);
                }

                else WinMessage = null;

                if (!libExceptionFlags.SuppressHandler) LibraryExceptionHandlers.InvokeExceptionHandler(this);
                if (handles.Length > 0) for (int i = 0; i < handles.Length; i++) CloseHandle(handles[i]);
            }

            public bool IsError { get; private set; }

            public string? FunctionName { get; private set; }
            public string? FilePath { get; private set; }
            public ulong? LineNumber { get; private set; }

            public bool IsWinError { get; private set; }
            public string? WinMessage { get; private set; }
        }
    }
}
