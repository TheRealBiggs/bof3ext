using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using unsafe DrawCursorSig = delegate* unmanaged[Cdecl]<short, short, void>;

namespace BoF3Ext.Patches; 


static partial class Gui {
	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static void DrawCursor(short x, short y) {
		var offset = ConfigManager.Instance.HalfRenderWidthDiff;

		Game.Gui.DrawCursor((short)(x + offset), y);
	}


	public static unsafe void Apply() {
		ApplyMenuGuiPatches();
		ApplyBattleGuiPatches();

		ProtectedMemory.WriteCall(0x596116, (DrawCursorSig)(&DrawCursor));
		ProtectedMemory.WriteCall(0x596144, (DrawCursorSig)(&DrawCursor));
	}
}