using System;
using System.IO;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Threading;

namespace Loader;


public static class EntryPoint {
	static readonly Lock initialiseLock = new();
	static int state;


	[UnmanagedCallersOnly(EntryPoint = "Initialise", CallConvs = [typeof(CallConvCdecl)])]
	public static void Initialise() {
		InitialiseLoader();
	}

	public static void InitialiseLoader() {
		if (Volatile.Read(ref state) == 2)
			return;

		lock (initialiseLock) {
			// Allow a loaded mod to call a forwarded export during initialisation.
			if (state != 0)
				return;

			state = 1;

			try {
				DirectDraw.Load();
				LoadMods();
				Volatile.Write(ref state, 2);
			} catch (Exception exception) {
				Environment.FailFast("Could not initialise the DirectDraw loader.", exception);
			}
		}
	}


	static unsafe void LoadMods() {
		if (!File.Exists("loader.cfg"))
			return;

		var lines = File.ReadAllLines("loader.cfg");

		foreach (var line in lines) {
			if (line.Length == 0 || line[0] == '#')
				continue;

			nint module;

			fixed (char* path = line)
				module = Win32.LoadLibrary(path);

			if (module == 0)
				continue;

			// Native DLLs initialise through DllMain; NativeAOT mods can export Initialise.
			fixed (byte* name = "Initialise\0"u8) {
				var initialise = Win32.GetProcAddress(module, name);

				if (initialise != 0)
					((delegate* unmanaged[Cdecl]<void>)initialise)();
			}
		}
	}
}