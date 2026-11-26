using System;
using System.Runtime.CompilerServices;

using BoF3Ext.Wrappers;

namespace BoF3Ext;


static unsafe partial class ProtectedMemory {
	const uint PAGE_EXECUTE_READWRITE = 0x40;


	public static void Write<T>(nuint address, T value) where T : unmanaged {
		var size = (nuint)Unsafe.SizeOf<T>();

		if (!VirtualProtectLogError(address, size, PAGE_EXECUTE_READWRITE, out var oldProtection))
			return;

		Unsafe.WriteUnaligned((void*)address, value);

		VirtualProtectLogError(address, size, oldProtection, out _);

		if ((oldProtection & 0xF0) != 0)
			Win32.FlushInstructionCache(Win32.GetCurrentProcess(), (void*)address, size);
	}

	public static void Write<T>(nuint address, T[] value) where T : unmanaged {
		if (value.Length == 0)
			return;

		var size = (nuint)((uint)Unsafe.SizeOf<T>() * (uint)value.Length);

		if (!VirtualProtectLogError(address, size, PAGE_EXECUTE_READWRITE, out var oldProtection))
			return;

		value.AsSpan().CopyTo(new((void*)address, value.Length));

		VirtualProtectLogError(address, size, oldProtection, out _);

		if ((oldProtection & 0xF0) != 0)
			Win32.FlushInstructionCache(Win32.GetCurrentProcess(), (void*)address, size);
	}

	public static void Nop(nuint address, nuint size) {
		if (size == 0)
			return;

		if (!VirtualProtectLogError(address, size, PAGE_EXECUTE_READWRITE, out var oldProtection))
			return;

		new Span<byte>((void*)address, (int)size).Fill(0x90);

		VirtualProtectLogError(address, size, oldProtection, out _);

		if ((oldProtection & 0xF0) != 0)
			Win32.FlushInstructionCache(Win32.GetCurrentProcess(), (void*)address, size);
	}

	public static void WriteCall(nuint address, void* functionPointer) {
		if (!VirtualProtectLogError(address, 5, PAGE_EXECUTE_READWRITE, out var oldProtection))
			return;

		var code = stackalloc byte[5];  // CALL rel32
		code[0] = 0xE8;
		*(nuint*)&code[1] = (nuint)functionPointer - (address + 5);

		Unsafe.CopyBlockUnaligned((void*)address, code, 5);

		VirtualProtectLogError(address, 5, oldProtection, out _);

		if ((oldProtection & 0xF0) != 0)
			Win32.FlushInstructionCache(Win32.GetCurrentProcess(), (void*)address, 5);
	}

	public static void WriteCall(nuint address, void* functionPointer, nuint trailingNopCount) {
		WriteCall(address, functionPointer);
		Nop(address + 5, trailingNopCount);
	}


	[MethodImpl(MethodImplOptions.AggressiveInlining)]
	static bool VirtualProtectLogError(nuint address, nuint size, uint newProtection, out uint oldProtection) {
		if (!Win32.VirtualProtect((void*)address, size, newProtection, out oldProtection)) {
			Console.WriteErrorLine($"Failed to change protection of address 0x{address:X8}, size: {size}");

			return false;
		}

		return true;
	}
}
