using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;


namespace Loader;


public static unsafe class DirectDraw {
	private static delegate* unmanaged[Stdcall]<void*, void*, int> directDrawEnumerateA;
	private static delegate* unmanaged[Stdcall]<void*, void*, void*, int> directDrawCreate;
	private static delegate* unmanaged[Stdcall]<void> releaseDDThreadLock;
	private static delegate* unmanaged[Stdcall]<void> acquireDDThreadLock;
	private static delegate* unmanaged[Stdcall]<uint, uint> ddInternalUnlock;
	private static delegate* unmanaged[Stdcall]<uint, uint, uint> ddInternalLock;
	private static delegate* unmanaged[Stdcall]<void*, void*, int> d3DParseUnknownCommand;


	internal static void Load() {
		// Keep the system DLL loaded for the lifetime of the process.
		char* path = stackalloc char[512];
		var length = Win32.GetSystemDirectory(path, 512);

		if (length == 0 || length + 11 > 512)
			throw new InvalidOperationException("Could not locate the system directory.");

		"\\ddraw.dll\0".AsSpan().CopyTo(new Span<char>(path + length, 11));
		var module = Win32.LoadLibrary(path);

		if (module == 0)
			throw new InvalidOperationException("Could not load system ddraw.dll.");

		directDrawEnumerateA = (delegate* unmanaged[Stdcall]<void*, void*, int>)GetExport(module, "DirectDrawEnumerateA\0"u8);
		directDrawCreate = (delegate* unmanaged[Stdcall]<void*, void*, void*, int>)GetExport(module, "DirectDrawCreate\0"u8);
		releaseDDThreadLock = (delegate* unmanaged[Stdcall]<void>)GetExport(module, "ReleaseDDThreadLock\0"u8);
		acquireDDThreadLock = (delegate* unmanaged[Stdcall]<void>)GetExport(module, "AcquireDDThreadLock\0"u8);
		ddInternalUnlock = (delegate* unmanaged[Stdcall]<uint, uint>)GetExport(module, "DDInternalUnlock\0"u8);
		ddInternalLock = (delegate* unmanaged[Stdcall]<uint, uint, uint>)GetExport(module, "DDInternalLock\0"u8);
		d3DParseUnknownCommand = (delegate* unmanaged[Stdcall]<void*, void*, int>)GetExport(module, "D3DParseUnknownCommand\0"u8);
	}

	private static nint GetExport(nint module, ReadOnlySpan<byte> name) {
		fixed (byte* pointer = name) {
			var address = Win32.GetProcAddress(module, pointer);

			if (address == 0)
				throw new InvalidOperationException("A required DirectDraw export is missing.");

			return address;
		}
	}


	[UnmanagedCallersOnly(EntryPoint = "DirectDrawEnumerateA", CallConvs = [typeof(CallConvStdcall)])]
	public static int DirectDrawEnumerateA(void* callback, void* context) {
		EntryPoint.InitialiseLoader();
		return directDrawEnumerateA(callback, context);
	}

	[UnmanagedCallersOnly(EntryPoint = "DirectDrawCreate", CallConvs = [typeof(CallConvStdcall)])]
	public static int DirectDrawCreate(void* guid, void* directDraw, void* outer) {
		EntryPoint.InitialiseLoader();
		return directDrawCreate(guid, directDraw, outer);
	}

	[UnmanagedCallersOnly(EntryPoint = "ReleaseDDThreadLock", CallConvs = [typeof(CallConvStdcall)])]
	public static void ReleaseDDThreadLock() {
		EntryPoint.InitialiseLoader();
		releaseDDThreadLock();
	}

	[UnmanagedCallersOnly(EntryPoint = "AcquireDDThreadLock", CallConvs = [typeof(CallConvStdcall)])]
	public static void AcquireDDThreadLock() {
		EntryPoint.InitialiseLoader();
		acquireDDThreadLock();
	}

	[UnmanagedCallersOnly(EntryPoint = "DDInternalUnlock", CallConvs = [typeof(CallConvStdcall)])]
	public static uint DDInternalUnlock(uint arg) {
		EntryPoint.InitialiseLoader();
		return ddInternalUnlock(arg);
	}

	[UnmanagedCallersOnly(EntryPoint = "DDInternalLock", CallConvs = [typeof(CallConvStdcall)])]
	public static uint DDInternalLock(uint arg1, uint arg2) {
		EntryPoint.InitialiseLoader();
		return ddInternalLock(arg1, arg2);
	}

	[UnmanagedCallersOnly(EntryPoint = "D3DParseUnknownCommand", CallConvs = [typeof(CallConvStdcall)])]
	public static int D3DParseUnknownCommand(void* command, void* returnCommand) {
		EntryPoint.InitialiseLoader();
		return d3DParseUnknownCommand(command, returnCommand);
	}
}
