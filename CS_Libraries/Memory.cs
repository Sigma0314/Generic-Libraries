using System.Runtime.InteropServices;
using System.Diagnostics;

namespace Lib
{
    namespace Memory
    {
        class External
        {
            private enum OpenProcessFlags : uint
            {
                CreateThread = 0x0002,
                QueryInformation = 0x0400,
                VmOperation = 0x0008,
                VmRead = 0x0010,
                VmWrite = 0x0020
            }

            public enum MemoryPageProtection : uint
            {
                ExecuteRead = 0x20,
                ExecuteReadWrite = 0x40,
                Readonly = 0x2,
                ReadWrite = 0x4,
                NoAccess = 0x01
            }

            public enum AllocationType : uint
            {
                Commit = 0x00001000,
                Reserve = 0x00002000,
                Reset = 0x00080000,
                ResetUndo = 0x1000000,
                LargePages = 0x20000000,
                Physical = 0x00400000,
                TopDown = 0x00100000
            }

            public enum FreeType : uint
            {
                Decommit = 0x00004000,
                Release = 0x00008000,
                CoalescePlaceholders = 0x00000001,
                PreservePlaceholder = 0x00000002
            }

            struct MemoryBasicInformation
            {
                public IntPtr BaseAddress;
                public IntPtr AllocationBase;
                public uint AllocationProtect;
                public ushort PartitionId;
                public ulong RegionSize;
                public uint State;
                public uint Protect;
                public uint Type;
            }

            [DllImport("kernel32.dll", CallingConvention = CallingConvention.StdCall, SetLastError = true)]
            private static extern IntPtr OpenProcess(OpenProcessFlags dwDesiredAccess, bool bInheritHandle, uint dwProcessId);

            [DllImport("kernel32.dll", CallingConvention = CallingConvention.StdCall, SetLastError = true)]
            private static extern bool CloseHandle(IntPtr hObject);

            [DllImport("kernel32.dll", CallingConvention = CallingConvention.StdCall, SetLastError = true)]
            private static extern bool ReadProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, [Out] byte[] lpBuffer, int dwSize, out ulong lpNumberOfBytesRead);

            [DllImport("kernel32.dll", CallingConvention = CallingConvention.StdCall, SetLastError = true)]
            private static extern bool WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, [In] byte[] lpBuffer, int dwSize, out ulong lpNumberOfBytesRead);

            [DllImport("kernel32.dll", CallingConvention = CallingConvention.StdCall, SetLastError = true)]
            private static extern bool VirtualProtectEx(IntPtr hProcess, IntPtr lpAddress, ulong dwSize, MemoryPageProtection dwNewProtect, out MemoryPageProtection lpOldProtect);

            [DllImport("kernel32.dll", CallingConvention = CallingConvention.StdCall, SetLastError = true)]
            private static extern ulong VirtualAllocEx(IntPtr hProcess, IntPtr? lpAddress, ulong dwSize, AllocationType dwAllocationType, MemoryPageProtection dwProtect);

            [DllImport("kernel32.dll", CallingConvention = CallingConvention.StdCall, SetLastError = true)]
            private static extern bool VirtualFreeEx(IntPtr hProcess, IntPtr lpAddress, uint? dwSize, FreeType dwFreeType);

            [DllImport("kernel32.dll", CallingConvention = CallingConvention.StdCall, SetLastError = true)]
            private static extern bool VirtualQueryEx(IntPtr hProcess, IntPtr lpAddress, out MemoryBasicInformation mbi, uint dwLength);

            private ulong _Base;
            private uint _Size;
            private uint _ProcessId;
            private IntPtr _Handle;

            public ulong ModuleBase { get { return _Base; } }
            public uint ModuleSize { get { return _Size; } }

            public External(string _ProcessName, string? _ModuleName = null)
            {
                Process[] procArr = Process.GetProcessesByName(_ProcessName);
                Debug.Assert(procArr.Length > 0);

                Process proc = procArr[0];
                _ProcessId = (uint)proc.Id;

                ProcessModule? mod = null;
                if (_ModuleName != null)
                {
                    for (int i = 0; i < proc.Modules.Count; i++)
                        if (proc.Modules[i].ModuleName == _ModuleName)
                        {
                            mod = proc.Modules[i];
                            break;
                        }
                }
                else
                {
                    _ProcessName += ".exe";
                    for (int i = 0; i < proc.Modules.Count; i++)
                        if (proc.Modules[i].ModuleName == _ProcessName)
                        {
                            mod = proc.Modules[i];
                            break;
                        }
                }

                Debug.Assert(mod != null);

                _Base = (ulong)mod.BaseAddress;
                _Size = (uint)mod.ModuleMemorySize;

                _Handle = OpenProcess(OpenProcessFlags.CreateThread | OpenProcessFlags.QueryInformation | OpenProcessFlags.VmOperation | OpenProcessFlags.VmRead | OpenProcessFlags.VmWrite, false, _ProcessId);
                Debug.Assert(_Handle != IntPtr.Zero);
            }

            ~External()
            {
                CloseHandle(_Handle);
            }

            public bool Read<T>(ulong _Address, out T _Return) where T : struct
            {
                int nSize = Marshal.SizeOf(default(T));

                byte[] buffer = new byte[nSize];

                ulong temp;
                ReadProcessMemory(_Handle, (IntPtr)_Address, buffer, nSize, out temp);

                // _Return = (T)buffer;
                _Return = default(T);
                Console.WriteLine(typeof(T).FullName);

                return true;
            }
        }
    }
}
