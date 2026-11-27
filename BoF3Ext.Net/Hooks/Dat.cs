using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BoF3Ext.Hooks;


static class Dat {
	public static int LastLoadedDatFileId;


	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void LoadDatFileHook(int id) {
		Console.WriteLine($"LoadDatFile: {id} - {Game.Dat.FileNames[id]}");

		if (id == 2 || (id >= 210 && id <= 251) || id == 549)
			LastLoadedDatFileId = id;

		Game.Dat.OriginalLoadDatFile(id);
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void LoadDatFile2Hook(int id) {
		Console.WriteLine($"LoadDatFile2: {id} - {Game.Dat.FileNames[id]}");

		Game.Dat.OriginalLoadDatFile2(id);
	}


	public static unsafe void Enable() {
		Game.Dat.HookLoadDatFile(&LoadDatFileHook);
		Game.Dat.HookLoadDatFile2(&LoadDatFile2Hook);
	}
}