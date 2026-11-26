using System.Runtime.InteropServices;


namespace Loader;


static unsafe partial class Win32 {
	[LibraryImport("kernel32", EntryPoint = "GetSystemDirectoryW")]
	internal static partial uint GetSystemDirectory(char* buffer, uint size);

	[LibraryImport("kernel32", EntryPoint = "LoadLibraryW")]
	internal static partial nint LoadLibrary(char* path);

	[LibraryImport("kernel32", EntryPoint = "GetProcAddress")]
	internal static partial nint GetProcAddress(nint module, byte* name);
}
