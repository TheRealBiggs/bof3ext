using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BoF3Ext.Hooks;


static class Overworld {
	public static nint LocationPanelActor;


	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void UpdateLocationPanelHook() {
		LocationPanelActor = (nint)Unsafe.AsPointer(ref Game.World.CurrentActor);

		Game.Overworld.OriginalUpdateLocationPanel();
	}


	public static unsafe void Enable() {
		Game.Overworld.HookUpdateLocationPanel(&UpdateLocationPanelHook);
	}
}