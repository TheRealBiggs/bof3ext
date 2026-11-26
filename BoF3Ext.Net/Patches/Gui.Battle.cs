
using unsafe DrawCenteredTextSig = delegate* unmanaged[Cdecl]<short, short, uint, byte, BoF3Ext.GameString, nint>;

namespace BoF3Ext.Patches;


static partial class Gui {
	static unsafe void ApplyBattleGuiPatches() {
		var diff = ConfigManager.Instance.RenderWidthDiff;
		var positions = Game.Gui.EnemyBattlePanelPositions;

		positions[0].x += (ushort)diff;
		positions[3].x += (ushort)diff;

		// Fix text centering for category in battle inventory window
		ProtectedMemory.WriteCall(0x59CF7F, (DrawCenteredTextSig)(&DrawCenteredText_Sub2));

		// Fix text centering for category in battle skill window
		ProtectedMemory.WriteCall(0x59D450, (DrawCenteredTextSig)(&DrawCenteredText_Sub2));
	}
}