using System;
using System.Runtime.InteropServices;

namespace BoF3Ext.Wrappers;


public static unsafe partial class MinHook {
	[LibraryImport("minhook")]
	private static partial int MH_Initialize();

	[LibraryImport("minhook")]
	private static partial int MH_CreateHook(nint target, nint detour, nint* original);

	[LibraryImport("minhook")]
	private static partial int MH_EnableHook(nint target);

	[LibraryImport("minhook")]
	private static partial int MH_RemoveHook(nint target);


	public static nint Create(nint address, nint hook) {
		var status = MH_Initialize();

		if (status != 0 && status != 1)
			throw new InvalidOperationException($"Failed to initialise MinHook: {status}.");

		nint original;
		status = MH_CreateHook(address, hook, &original);

		if (status != 0)
			throw new InvalidOperationException($"Failed to create hook at 0x{address:X}: {status}.");

		return original;
	}

	public static void Enable(nint address) {
		var status = MH_EnableHook(address);

		if (status != 0)
			throw new InvalidOperationException($"Failed to enable hook at 0x{address:X}: {status}.");
	}

	public static void Remove(nint address) {
		var status = MH_RemoveHook(address);

		if (status != 0)
			throw new InvalidOperationException($"Failed to remove hook at 0x{address:X}: {status}.");
	}
}
