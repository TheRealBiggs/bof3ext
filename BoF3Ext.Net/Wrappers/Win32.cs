using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BoF3Ext.Wrappers;


static unsafe partial class Win32 {
	[LibraryImport("kernel32.dll", SetLastError = true)]
	[return: MarshalAs(UnmanagedType.Bool)]
	public static partial bool VirtualProtect(void* address, nuint size, uint newProtect, out uint oldProtect);

	[LibraryImport("kernel32.dll", SetLastError = true)]
	[return: MarshalAs(UnmanagedType.Bool)]
	public static partial bool FlushInstructionCache(nint process, void* address, nuint size);

	[LibraryImport("kernel32.dll")]
	public static partial nint GetCurrentProcess();

	[LibraryImport("kernel32.dll")]
	[return: MarshalAs(UnmanagedType.Bool)]
	public static partial bool AllocConsole();

	[LibraryImport("kernel32.dll")]
	[return: MarshalAs(UnmanagedType.Bool)]
	public static partial bool FreeConsole();

	[LibraryImport("kernel32.dll")]
	[return: MarshalAs(UnmanagedType.Bool)]
	public static partial bool SetConsoleOutputCP(uint codePage);

	[LibraryImport("kernel32.dll", SetLastError = true)]
	public static partial void* VirtualAlloc(void* address, nuint size, uint allocationType, uint protection);


	[LibraryImport("ucrtbase.dll", EntryPoint = "__acrt_iob_func")]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial nint GetStream(uint index);

	[LibraryImport("ucrtbase.dll", StringMarshalling = StringMarshalling.Utf8)]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial int freopen_s(out nint reopened, string filename, string mode, nint stream);

	[LibraryImport("ucrtbase.dll", StringMarshalling = StringMarshalling.Utf8)]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial int fputs(string text, nint stream);

	[LibraryImport("ucrtbase.dll")]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial int fputc(int character, nint stream);

	[LibraryImport("ucrtbase.dll")]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial int fflush(nint stream);

	[LibraryImport("ucrtbase.dll")]
	[UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
	public static partial int fclose(nint stream);
}